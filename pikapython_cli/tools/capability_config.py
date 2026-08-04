#!/usr/bin/env python3
# SPEC: PJ2026-0501 kernel v0.24; capability configuration prebuild.
import hashlib
import json
import re
from pathlib import Path

import yaml


CONFIG_SCHEMA = "pika.capability-config/v1"
PREBUILD_SCHEMA = "pika.capability-prebuild/v1"
CAPABILITY_ID = re.compile(r"^[a-z][a-z0-9]*(?:[.-][a-z0-9]+)*$")
PROFILE_ID = re.compile(r"^[a-z][a-z0-9]*(?:-[a-z0-9]+)*$")
MACRO_NAME = re.compile(r"^[A-Z][A-Z0-9_]*_ENABLE$")
MAX_C_INTEGER = 4294967295
HTTP_CAPABILITY = "library.http-client"
HTTP_CONFIG_MODULE = "pika_http_config.py"
MQTT_CAPABILITY = "library.mqtt-client"
MQTT_CONFIG_MODULE = "pika_mqtt_config.py"
GC_CAPABILITY = "runtime.gc"
FRONTEND_LIMIT_MACROS = {
    "callDepth": "PIKA_FRONTEND_CALL_DEPTH_LIMIT",
    "dynamicSequenceElements": "PIKA_FRONTEND_DYNAMIC_SEQUENCE_ELEMENT_RESERVE",
    "functionCount": "PIKA_FRONTEND_FUNCTION_LIMIT",
}
GC_LIMIT_MACROS = {
    "arenaBytes": "PIKA_GC_ARENA_BYTES",
    "arenaTriggerPercent": "PIKA_GC_ARENA_TRIGGER_PERCENT",
    "bindingTriggerPercent": "PIKA_GC_BINDING_TRIGGER_PERCENT",
    "objectSlotCapacity": "PIKA_GC_OBJECT_SLOT_CAPACITY",
    "slotTriggerPercent": "PIKA_GC_SLOT_TRIGGER_PERCENT",
}
HTTP_LIMIT_MACROS = {
    "bodyBytes": "PIKA_HTTP_BODY_LIMIT",
    "headerBytes": "PIKA_HTTP_HEADER_BYTES_LIMIT",
    "headerCount": "PIKA_HTTP_HEADER_COUNT_LIMIT",
    "pathBytes": "PIKA_HTTP_PATH_LIMIT",
    "recvBytes": "PIKA_HTTP_RECV_LIMIT",
    "urlBytes": "PIKA_HTTP_URL_LIMIT",
}
MQTT_LIMIT_MACROS = {
    "clientIdBytes": "PIKA_MQTT_CLIENT_ID_LIMIT",
    "packetBytes": "PIKA_MQTT_PACKET_LIMIT",
    "payloadBytes": "PIKA_MQTT_PAYLOAD_LIMIT",
    "remainingLengthBytes": "PIKA_MQTT_REMAINING_LENGTH_BYTES",
    "topicBytes": "PIKA_MQTT_TOPIC_LIMIT",
}


class CapabilityConfigError(Exception):
    def __init__(self, code, message):
        super().__init__(message)
        self.code = code


def require_sorted_strings(value, field, allow_empty=False):
    if (
        not isinstance(value, list)
        or (not value and not allow_empty)
        or not all(isinstance(item, str) and item for item in value)
        or value != sorted(value)
        or len(value) != len(set(value))
    ):
        raise CapabilityConfigError(
            "invalid_config",
            "%s must be a sorted unique string list" % field,
        )
    return value


def require_resource_limit(value, field):
    if (
        not isinstance(value, int)
        or isinstance(value, bool)
        or value < 1
        or value > MAX_C_INTEGER
    ):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "%s must be an integer in 1..%d" % (field, MAX_C_INTEGER),
        )
    return value


def load_resource_limits(value):
    if not isinstance(value, dict):
        raise CapabilityConfigError(
            "invalid_resource_limit", "resourceLimits must be an object"
        )
    if "gc" not in value:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.gc is required",
        )
    if "frontend" not in value:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.frontend is required",
        )
    if "httpClient" not in value:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.httpClient is required",
        )
    if set(value) != {
        "frontend",
        "gc",
        "httpClient",
        "mqttClient",
    }:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits contains unsupported groups",
        )
    frontend = value["frontend"]
    if (
        not isinstance(frontend, dict)
        or set(frontend) != set(FRONTEND_LIMIT_MACROS)
    ):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.frontend must contain exactly the supported fields",
        )
    frontend_call_depth = require_resource_limit(
        frontend["callDepth"],
        "resourceLimits.frontend.callDepth",
    )
    frontend_dynamic_sequence_elements = require_resource_limit(
        frontend["dynamicSequenceElements"],
        "resourceLimits.frontend.dynamicSequenceElements",
    )
    frontend_function_count = require_resource_limit(
        frontend["functionCount"],
        "resourceLimits.frontend.functionCount",
    )
    if frontend_call_depth > 65535:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.frontend.callDepth must be in 1..65535",
        )
    if frontend_dynamic_sequence_elements > 255:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.frontend.dynamicSequenceElements must be in 1..255",
        )
    if frontend_function_count > 65534:
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.frontend.functionCount must be in 1..65534",
        )
    gc = value["gc"]
    if not isinstance(gc, dict) or set(gc) != set(GC_LIMIT_MACROS):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.gc must contain exactly the supported fields",
        )
    for field in sorted(GC_LIMIT_MACROS):
        limit = require_resource_limit(
            gc[field], "resourceLimits.gc.%s" % field
        )
        if field.endswith("TriggerPercent") and limit > 100:
            raise CapabilityConfigError(
                "invalid_resource_limit",
                "resourceLimits.gc.%s must be in 1..100" % field,
            )
        if field == "objectSlotCapacity" and limit > 65535:
            raise CapabilityConfigError(
                "invalid_resource_limit",
                "resourceLimits.gc.objectSlotCapacity must be in 1..65535",
            )
    http_client = value["httpClient"]
    if not isinstance(http_client, dict):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.httpClient must be an object",
        )
    for field in sorted(HTTP_LIMIT_MACROS):
        if field not in http_client:
            raise CapabilityConfigError(
                "invalid_resource_limit",
                "resourceLimits.httpClient.%s is required" % field,
            )
    if set(http_client) != set(HTTP_LIMIT_MACROS):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.httpClient contains unsupported fields",
        )
    mqtt_client = value.get("mqttClient")
    if not isinstance(mqtt_client, dict) or set(mqtt_client) != set(MQTT_LIMIT_MACROS):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "resourceLimits.mqttClient must contain exactly the supported fields",
        )
    return {
        "frontend": {
            "callDepth": frontend_call_depth,
            "dynamicSequenceElements": frontend_dynamic_sequence_elements,
            "functionCount": frontend_function_count,
        },
        "gc": {
            field: gc[field] for field in sorted(GC_LIMIT_MACROS)
        },
        "httpClient": {
            field: require_resource_limit(
                http_client[field],
                "resourceLimits.httpClient.%s" % field,
            )
            for field in sorted(HTTP_LIMIT_MACROS)
        },
        "mqttClient": {
            field: require_resource_limit(
                mqtt_client[field],
                "resourceLimits.mqttClient.%s" % field,
            )
            for field in sorted(MQTT_LIMIT_MACROS)
        },
    }


def merge_profile_resource_limits(defaults, override, profile_id):
    if not isinstance(override, dict):
        raise CapabilityConfigError(
            "invalid_resource_limit",
            "profiles.%s.resourceLimits must be an object" % profile_id,
        )
    supported_fields = {
        "frontend": set(FRONTEND_LIMIT_MACROS),
        "gc": set(GC_LIMIT_MACROS),
        "httpClient": set(HTTP_LIMIT_MACROS),
        "mqttClient": set(MQTT_LIMIT_MACROS),
    }
    merged = {
        group: dict(values) for group, values in defaults.items()
    }
    for group, values in override.items():
        if group not in supported_fields:
            raise CapabilityConfigError(
                "invalid_resource_limit",
                "profiles.%s.resourceLimits contains unsupported group %s"
                % (profile_id, group),
            )
        if not isinstance(values, dict) or not values:
            raise CapabilityConfigError(
                "invalid_resource_limit",
                "profiles.%s.resourceLimits.%s must be a non-empty object"
                % (profile_id, group),
            )
        unknown = set(values) - supported_fields[group]
        if unknown:
            raise CapabilityConfigError(
                "invalid_resource_limit",
                "profiles.%s.resourceLimits.%s contains unsupported fields"
                % (profile_id, group),
            )
        merged[group].update(values)
    return load_resource_limits(merged)


def capability_macro(capability_id, descriptor):
    explicit = descriptor.get("macro")
    if explicit is not None:
        if not isinstance(explicit, str) or MACRO_NAME.fullmatch(explicit) is None:
            raise CapabilityConfigError(
                "invalid_macro", "invalid macro for %s" % capability_id
            )
        return explicit
    normalized = re.sub(r"[^A-Za-z0-9]+", "_", capability_id).upper()
    return "PIKA_CAPABILITY_%s_ENABLE" % normalized


def load_config(path):
    try:
        raw = path.read_bytes()
        value = yaml.safe_load(raw.decode("utf-8"))
    except (OSError, UnicodeError, yaml.YAMLError) as error:
        raise CapabilityConfigError(
            "config_read_failed", "cannot read capability config: %s" % error
        ) from error
    if not isinstance(value, dict):
        raise CapabilityConfigError("invalid_config", "config must be an object")
    if "resourceLimits" not in value:
        raise CapabilityConfigError(
            "invalid_resource_limit", "resourceLimits is required"
        )
    if set(value) != {
        "schema",
        "capabilities",
        "profiles",
        "resourceLimits",
    }:
        raise CapabilityConfigError("invalid_config", "invalid top-level fields")
    if value["schema"] != CONFIG_SCHEMA:
        raise CapabilityConfigError("unsupported_schema", "unsupported config schema")
    capabilities = value["capabilities"]
    profiles = value["profiles"]
    if not isinstance(capabilities, dict) or not capabilities:
        raise CapabilityConfigError("invalid_config", "capabilities must be an object")
    if not isinstance(profiles, dict) or not profiles:
        raise CapabilityConfigError("invalid_config", "profiles must be an object")
    value["resourceLimits"] = load_resource_limits(value["resourceLimits"])

    macro_owners = {}
    for capability_id, descriptor in capabilities.items():
        if (
            not isinstance(capability_id, str)
            or CAPABILITY_ID.fullmatch(capability_id) is None
            or not isinstance(descriptor, dict)
            or not {"dependencies", "supported"}.issubset(descriptor)
            or not set(descriptor).issubset({"dependencies", "supported", "macro"})
            or not isinstance(descriptor["supported"], bool)
        ):
            raise CapabilityConfigError(
                "invalid_capability", "invalid capability: %s" % capability_id
            )
        dependencies = require_sorted_strings(
            descriptor["dependencies"],
            "capabilities.%s.dependencies" % capability_id,
            allow_empty=True,
        )
        for dependency in dependencies:
            if CAPABILITY_ID.fullmatch(dependency) is None:
                raise CapabilityConfigError(
                    "invalid_dependency", "invalid dependency: %s" % dependency
                )
        macro = capability_macro(capability_id, descriptor)
        if macro in macro_owners:
            raise CapabilityConfigError(
                "duplicate_macro",
                "%s and %s map to %s"
                % (macro_owners[macro], capability_id, macro),
            )
        macro_owners[macro] = capability_id

    for capability_id, descriptor in capabilities.items():
        for dependency in descriptor["dependencies"]:
            if dependency not in capabilities:
                raise CapabilityConfigError(
                    "unknown_dependency",
                    "%s depends on unknown capability %s"
                    % (capability_id, dependency),
                )

    for profile_id, descriptor in profiles.items():
        descriptor_fields = {"roots", "syntaxLevel"}
        if isinstance(descriptor, dict) and "resourceLimits" in descriptor:
            descriptor_fields.add("resourceLimits")
        if (
            not isinstance(profile_id, str)
            or PROFILE_ID.fullmatch(profile_id) is None
            or not isinstance(descriptor, dict)
            or set(descriptor) != descriptor_fields
            or not isinstance(descriptor["syntaxLevel"], int)
            or isinstance(descriptor["syntaxLevel"], bool)
            or not 1 <= descriptor["syntaxLevel"] <= 8
        ):
            raise CapabilityConfigError(
                "invalid_profile", "invalid profile: %s" % profile_id
            )
        roots = require_sorted_strings(
            descriptor["roots"], "profiles.%s.roots" % profile_id
        )
        for root in roots:
            if root not in capabilities:
                raise CapabilityConfigError(
                    "unknown_capability",
                    "%s selects unknown capability %s" % (profile_id, root),
                )
        if "resourceLimits" in descriptor:
            descriptor["effectiveResourceLimits"] = merge_profile_resource_limits(
                value["resourceLimits"], descriptor.pop("resourceLimits"), profile_id
            )
    return value, raw


def resolve_closure(config, profile_id):
    profiles = config["profiles"]
    capabilities = config["capabilities"]
    if profile_id not in profiles:
        raise CapabilityConfigError(
            "unknown_profile", "unknown capability profile: %s" % profile_id
        )
    resolved = set()
    active = []

    def visit(capability_id):
        if capability_id in resolved:
            return
        if capability_id in active:
            cycle = active[active.index(capability_id):] + [capability_id]
            raise CapabilityConfigError(
                "dependency_cycle",
                "capability dependency cycle: %s" % " -> ".join(cycle),
            )
        descriptor = capabilities[capability_id]
        if not descriptor["supported"]:
            raise CapabilityConfigError(
                "unsupported_capability",
                "profile %s selects unsupported capability %s"
                % (profile_id, capability_id),
            )
        active.append(capability_id)
        for dependency in descriptor["dependencies"]:
            visit(dependency)
        active.pop()
        resolved.add(capability_id)

    for root in profiles[profile_id]["roots"]:
        visit(root)
    return sorted(resolved)


def build_manifest(config, raw, config_path, profile_id, closure):
    capabilities = config["capabilities"]
    closure_set = set(closure)
    macros = {}
    for capability_id in sorted(capabilities):
        macros[capability_id] = {
            "name": capability_macro(capability_id, capabilities[capability_id]),
            "value": 1 if capability_id in closure_set else 0,
        }
    effective_limits = config["profiles"][profile_id].get(
        "effectiveResourceLimits", config["resourceLimits"]
    )
    resource_limits = {"frontend": effective_limits["frontend"]}
    python_modules = []
    if GC_CAPABILITY in closure_set:
        resource_limits["gc"] = effective_limits["gc"]
    if HTTP_CAPABILITY in closure_set:
        resource_limits["httpClient"] = effective_limits["httpClient"]
        python_modules.append(HTTP_CONFIG_MODULE)
    if MQTT_CAPABILITY in closure_set:
        resource_limits["mqttClient"] = effective_limits["mqttClient"]
        python_modules.append(MQTT_CONFIG_MODULE)
    normalized = json.dumps(
        {
            "profile": profile_id,
            "syntaxLevel": config["profiles"][profile_id]["syntaxLevel"],
            "roots": config["profiles"][profile_id]["roots"],
            "closure": closure,
            "macros": macros,
            "pythonModules": python_modules,
            "resourceLimits": resource_limits,
        },
        ensure_ascii=True,
        separators=(",", ":"),
        sort_keys=True,
    ).encode("ascii")
    return {
        "schema": PREBUILD_SCHEMA,
        "profile": profile_id,
        "syntaxLevel": config["profiles"][profile_id]["syntaxLevel"],
        "roots": config["profiles"][profile_id]["roots"],
        "closure": closure,
        "macros": macros,
        "pythonModules": python_modules,
        "resourceLimits": resource_limits,
        "input": {
            "path": config_path.as_posix(),
            "sha256": hashlib.sha256(raw).hexdigest(),
        },
        "configDigest": hashlib.sha256(normalized).hexdigest(),
    }


def render_header(config, manifest):
    capabilities = config["capabilities"]
    macros = manifest["macros"]
    lines = [
        "/* Generated by capability-prebuild-cli.py. Do not edit. */",
        "#ifndef PIKA_CAPABILITY_CONFIG_H",
        "#define PIKA_CAPABILITY_CONFIG_H",
        "",
        "#define PIKA_CAPABILITY_SCHEMA_VERSION 1u",
        "#define PIKA_SYNTAX_LEVEL %du" % manifest["syntaxLevel"],
        '#define PIKA_CAPABILITY_CONFIG_DIGEST "%s"' % manifest["configDigest"],
        "",
    ]
    for capability_id in sorted(macros):
        descriptor = macros[capability_id]
        lines.append("#define %s %d" % (descriptor["name"], descriptor["value"]))
    frontend_limits = manifest["resourceLimits"]["frontend"]
    for field in sorted(FRONTEND_LIMIT_MACROS):
        macro = FRONTEND_LIMIT_MACROS[field]
        lines.extend([
            "",
            "#ifndef %s" % macro,
            "#define %s %du" % (macro, frontend_limits[field]),
            "#endif",
        ])
    gc_limits = manifest["resourceLimits"].get("gc")
    if gc_limits is not None:
        lines.extend(["", "#if %s" % macros[GC_CAPABILITY]["name"]])
        for field in sorted(GC_LIMIT_MACROS):
            lines.append(
                "#define %s %du"
                % (GC_LIMIT_MACROS[field], gc_limits[field])
            )
        lines.append("#endif")
    http_limits = manifest["resourceLimits"].get("httpClient")
    if http_limits is not None:
        lines.extend([
            "",
            "#if %s" % macros[HTTP_CAPABILITY]["name"],
        ])
        for field in sorted(HTTP_LIMIT_MACROS):
            lines.append(
                "#define %s %du"
                % (HTTP_LIMIT_MACROS[field], http_limits[field])
            )
        lines.append("#endif")
    mqtt_limits = manifest["resourceLimits"].get("mqttClient")
    if mqtt_limits is not None:
        lines.extend(["", "#if %s" % macros[MQTT_CAPABILITY]["name"]])
        for field in sorted(MQTT_LIMIT_MACROS):
            lines.append(
                "#define %s %du"
                % (MQTT_LIMIT_MACROS[field], mqtt_limits[field])
            )
        lines.append("#endif")
    lines.append("")
    for capability_id in sorted(macros):
        macro = macros[capability_id]["name"]
        lines.extend([
            "#if %s != 0 && %s != 1" % (macro, macro),
            '#error "%s must be 0 or 1"' % macro,
            "#endif",
        ])
    lines.append("")
    for capability_id in sorted(capabilities):
        macro = macros[capability_id]["name"]
        for dependency in capabilities[capability_id]["dependencies"]:
            dependency_macro = macros[dependency]["name"]
            lines.extend([
                "#if %s && !%s" % (macro, dependency_macro),
                '#error "%s requires %s"' % (macro, dependency_macro),
                "#endif",
            ])
    lines.extend(["", "#endif", ""])
    return "\n".join(lines)


def render_http_config(manifest):
    limits = manifest["resourceLimits"]["httpClient"]
    lines = [
        "# Generated by capability-prebuild-cli.py. Do not edit.",
        "",
    ]
    for field in sorted(HTTP_LIMIT_MACROS):
        lines.append(
            "%s = %d" % (HTTP_LIMIT_MACROS[field], limits[field])
        )
    lines.append("")
    return "\n".join(lines)


def render_mqtt_config(manifest):
    limits = manifest["resourceLimits"]["mqttClient"]
    lines = ["# Generated by capability-prebuild-cli.py. Do not edit.", ""]
    for field in sorted(MQTT_LIMIT_MACROS):
        lines.append("%s = %d" % (MQTT_LIMIT_MACROS[field], limits[field]))
    lines.append("")
    return "\n".join(lines)


def write_prebuild(config_path, profile_id, output):
    config, raw = load_config(config_path)
    closure = resolve_closure(config, profile_id)
    try:
        relative = config_path.resolve().relative_to(Path.cwd().resolve())
    except ValueError:
        relative = config_path.resolve()
    manifest = build_manifest(config, raw, relative, profile_id, closure)
    header = render_header(config, manifest)
    manifest_text = json.dumps(
        manifest, ensure_ascii=True, indent=2, sort_keys=True
    ) + "\n"
    try:
        output.mkdir(parents=True, exist_ok=True)
        files = [
            ("pika_capability_config.h", header, "ascii"),
            ("pika_capability_manifest.json", manifest_text, "ascii"),
        ]
        if HTTP_CONFIG_MODULE in manifest["pythonModules"]:
            files.append(
                (HTTP_CONFIG_MODULE, render_http_config(manifest), "ascii")
            )
        if MQTT_CONFIG_MODULE in manifest["pythonModules"]:
            files.append(
                (MQTT_CONFIG_MODULE, render_mqtt_config(manifest), "ascii")
            )
        for name, content, encoding in files:
            target = output / name
            temporary = output / (name + ".tmp")
            temporary.write_text(content, encoding=encoding, newline="\n")
            temporary.replace(target)
        if HTTP_CONFIG_MODULE not in manifest["pythonModules"]:
            stale = output / HTTP_CONFIG_MODULE
            if stale.exists():
                stale.unlink()
        if MQTT_CONFIG_MODULE not in manifest["pythonModules"]:
            stale = output / MQTT_CONFIG_MODULE
            if stale.exists():
                stale.unlink()
    except OSError as error:
        raise CapabilityConfigError(
            "output_write_failed", "cannot write capability prebuild: %s" % error
        ) from error
    return manifest
