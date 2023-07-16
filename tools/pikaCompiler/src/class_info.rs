use crate::decorator::Decorator;
use crate::import_info::ImportInfo;
use crate::method_info::MethodInfo;
use crate::my_string;
use crate::object_info::ObjectInfo;
use crate::script::Script;
use crate::version_info::VersionInfo;
use std::collections::BTreeMap;
#[derive(Debug)]
pub struct ClassInfo {
    pub this_file_name: String,
    pub this_class_name: String,
    pub this_class_name_without_file: String,
    pub super_class_name: String,
    pub method_list: BTreeMap<String, MethodInfo>,
    pub object_list: BTreeMap<String, ObjectInfo>,
    pub import_list: BTreeMap<String, ImportInfo>,
    pub script_list: Script,
    pub is_package: bool,
    pub is_top: bool,
}

impl ClassInfo {
    pub fn add_file_profix(file_name: &String, class_name: &String, is_package: bool) -> String {
        if is_package {
            return class_name.clone();
        }
        if file_name == "main" {
            return class_name.clone();
        } else if class_name == "BaseObj" || class_name == "TinyObj" {
            return class_name.clone();
        } else {
            return format!("{}_{}", file_name, class_name);
        }
    }

    pub fn new(file_name: &String, define: &String, is_package: bool) -> Option<ClassInfo> {
        let define = define.strip_prefix("class ").unwrap().to_string();
        let define = define.replace(" ", "");
        let mut super_class_name = match my_string::cut(&define, '(', ')') {
            Some(s) => s,
            None => "TinyObj".to_string(),
        };
        if super_class_name == "" {
            super_class_name = "TinyObj".to_string();
        }
        let super_class_name = match super_class_name.find(".") {
            None => ClassInfo::add_file_profix(&file_name, &super_class_name, is_package),
            Some(_x) => super_class_name.replace(".", "_"),
        };
        let mut this_calss_name = match my_string::get_first_token(&define, '(') {
            Some(s) => s,
            None => match my_string::get_first_token(&define, ':') {
                Some(s) => s,
                None => {
                    return None;
                }
            },
        };
        let this_class_name_without_file = this_calss_name.clone();
        this_calss_name = ClassInfo::add_file_profix(&file_name, &this_calss_name, is_package);
        let new_class_info = ClassInfo {
            this_file_name: file_name.clone(),
            this_class_name: this_calss_name,
            this_class_name_without_file: this_class_name_without_file,
            super_class_name: super_class_name,
            method_list: BTreeMap::new(),
            object_list: BTreeMap::new(),
            import_list: BTreeMap::new(),
            script_list: Script::new(),
            is_package: is_package,
            is_top: false,
        };
        return Some(new_class_info);
    }

    fn __push_method_or_constructor(
        &mut self,
        method_define: String,
        is_constructor: bool,
        decorator_list: Vec<Decorator>,
    ) {
        let mut method_info =
            match MethodInfo::new(&self.this_class_name, method_define, is_constructor) {
                Some(method) => method,
                None => return,
            };
        method_info.set_decorator_list(decorator_list);
        self.method_list
            .entry(method_info.name.clone())
            .or_insert(method_info);
    }
    pub fn push_method(&mut self, method_define: String, decorator_list: Vec<Decorator>) {
        return self.__push_method_or_constructor(method_define, false, decorator_list);
    }
    pub fn push_constructor(&mut self, method_define: String, decorator_list: Vec<Decorator>) {
        return self.__push_method_or_constructor(method_define, true, decorator_list);
    }
    // pub fn push_import(&mut self, import_define: String, file_name: &String) {
    //     let import_info = match ImportInfo::new(&self.this_class_name, import_define, &file_name) {
    //         Some(import) => import,
    //         None => return,
    //     };
    //     self.import_list
    //         .entry(import_info.import_class_name.clone())
    //         .or_insert(import_info);
    // }
    pub fn push_object(&mut self, object_define: String, file_name: &String) {
        let object_info = match ObjectInfo::new(&self.this_class_name, object_define, &file_name) {
            Some(object) => object,
            None => return,
        };
        self.object_list
            .entry(object_info.name.clone())
            .or_insert(object_info);
    }

    pub fn include(&self) -> String {
        let mut include = String::new();
        include.push_str(&format!("#include \"{}.h\"\n", self.this_class_name));
        include.push_str(&format!("#include \"{}.h\"\n", self.super_class_name));
        for (_, import_info) in self.import_list.iter() {
            include.push_str(&format!(
                "#include \"{}.h\"\n",
                import_info.import_class_name
            ));
        }
        for (_, object_info) in self.object_list.iter() {
            include.push_str(&format!(
                "#include \"{}.h\"\n",
                object_info.import_class_name
            ));
        }
        return include;
    }

    pub fn method_api_fn(&self) -> String {
        let mut method_impl = String::new();
        for (_, method_info) in self.method_list.iter() {
            method_impl.push_str(&method_info.method_fn_impl());
        }
        return method_impl;
    }

    pub fn script_fn(&self, version_info: VersionInfo) -> String {
        let mut script_fn = String::new();
        /* add pikaPythonInit function define */
        script_fn.push_str("volatile PikaObj *__pikaMain;\r\n");
        script_fn.push_str("PikaObj *pikaPythonInit(void){\r\n");
        /* print version info */
        script_fn.push_str(
            "    pika_platform_printf(\"======[pikapython packages installed]======\\r\\n\");\r\n",
        );
        script_fn.push_str("    pika_printVersion();\r\n");
        for (package_name, package_version) in version_info.package_list {
            script_fn.push_str(
                format!(
                    "    pika_platform_printf(\"{}=={}\\r\\n\");\r\n",
                    package_name, package_version
                )
                .as_str(),
            );
        }
        script_fn.push_str(
            "    pika_platform_printf(\"===========================================\\r\\n\");\r\n",
        );
        /* create the root object */
        script_fn.push_str("    PikaObj* pikaMain = newRootObj(\"pikaMain\", New_PikaMain);\r\n");
        script_fn.push_str("    __pikaMain = pikaMain;\r\n");
        /* use obj_run to run the script in main.pyi */
        script_fn.push_str("    extern unsigned char pikaModules_py_a[];\n");
        script_fn.push_str("    obj_linkLibrary(pikaMain, pikaModules_py_a);\n");
        script_fn.push_str("#if PIKA_INIT_STRING_ENABLE\n");
        script_fn.push_str("    obj_run(pikaMain,\n");
        /* get the origin script content */
        let script_content_origin = String::from(&self.script_list.content);
        /* filters for the script content */
        /*      remove void line */
        let script_content = String::from(&script_content_origin).replace("@BEGIN@@END@", "");
        /*      use \" instead of " */
        let script_content = script_content.replace("\\", "\\\\");
        let script_content = script_content.replace("\"", "\\\"");
        /* add begin and end */
        let script_content = script_content.replace("@BEGIN@", "            \"");
        let script_content = script_content.replace("@END@", "\\n\"\n");
        /* add the script content */
        script_fn.push_str(&script_content);
        /* add the END of script string */
        script_fn.push_str("            \"\\n\");\n");
        script_fn.push_str("#else \n");
        script_fn.push_str("    obj_runModule((PikaObj*)pikaMain, \"main\");\n");
        script_fn.push_str("#endif\n");
        script_fn.push_str("    return pikaMain;\r\n");
        script_fn.push_str("}\r\n\r\n");
        return script_fn;
    }

    pub fn new_class_fn(&self) -> String {
        let mut new_class_fn = String::new();
        let new_class_fn_head = format!("{}{{\n", self.new_class_fn_name());

        let class_def = format!("class_def({}){{\n", self.this_class_name);
        new_class_fn.push_str(&class_def);

        // new_class_fn.push_str("#ifdef _WIN32\n");
        new_class_fn.push_str("    __BEFORE_MOETHOD_DEF\n");
        // new_class_fn.push_str("#endif\n");

        let method_info_list = self.method_list.values().collect::<Vec<&MethodInfo>>();

        /* sort by name_hash */
        let mut method_info_list_sorted = method_info_list.clone();
        method_info_list_sorted.sort_by(|a, b| {
            let lhs = (a.name_hash) as u32;
            let rhs = (b.name_hash) as u32;
            return lhs.cmp(&rhs);
        });

        for method_info in method_info_list_sorted.iter() {
            new_class_fn.push_str(&method_info.get_define());
        }

        new_class_fn.push_str(&"};\n");
        let class_inhert = format!(
            "class_inhert({}, {});\n\n",
            self.this_class_name, self.super_class_name
        );

        new_class_fn.push_str(&class_inhert);

        new_class_fn.push_str(&new_class_fn_head);
        let derive = format!("    PikaObj *self = New_{}(args);\n", self.super_class_name);
        new_class_fn.push_str(&derive);

        for (_, object_info) in self.object_list.iter() {
            new_class_fn.push_str(&object_info.new_object_fn());
        }

        let obj_set_class = format!("    obj_setClass(self, {});\n", self.this_class_name);
        new_class_fn.push_str(&obj_set_class);

        new_class_fn.push_str("    return self;\n");
        new_class_fn.push_str("}\n");
        return new_class_fn;
    }

    pub fn new_class_fn_name(&self) -> String {
        return format!("PikaObj *New_{}(Args *args)", self.this_class_name);
    }

    pub fn method_impl_declear(&self) -> String {
        let mut method_fn_declear = String::new();
        for (_, method_info) in self.method_list.iter() {
            method_fn_declear.push_str(&method_info.method_impl_declear());
        }
        return method_fn_declear;
    }
}
