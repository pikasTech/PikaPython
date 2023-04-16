import os
from pathlib import Path


def collect_stack_usage_info():
    stack_usage_data = []

    for root, _, files in os.walk("./build/package/pikascript/CMakeFiles/pikascript-core.dir/pikascript-core"):
        for file in files:
            if file.endswith(".su"):
                su_path = Path(root) / file
                # print(f"Processing file: {su_path}")
                with open(su_path) as f:
                    for line in f.readlines():
                        # '/root/pikascript/port/linux/package/pikascript/pikascript-lib/pika_libc/pika_vsnprintf.c:184:38:get_bit_access'
                        items = line.strip().split()
                        if len(items) == 3:
                            location, stack_size, storage_type = items[0], items[1], items[2]
                            location_items = location.split(":")
                            if len(location_items) == 4:
                                file_name, line_number, column_number, function = location_items
                        else:
                            print(
                                f"Skipping line due to incorrect format: {line.strip()}")
                            continue
                        stack_usage_data.append(
                            (file_name, function, int(stack_size), storage_type))

    return stack_usage_data


def generate_report(stack_usage_data):
    sorted_data = sorted(stack_usage_data, key=lambda x: x[2], reverse=True)

    report = "Stack usage report (sorted by stack size):\n\n"
    report += "{:<10}  {:<40}  {:<15}\n".format(
        "Stack Size", "Function", "File")
    report += "-" * 75 + "\n"

    for file_name, function, stack_size, storage_type in sorted_data:
        report += "{:<10}  {:<40}  {:<15}\n".format(
            stack_size, function, os.path.basename(file_name))

    return report


if __name__ == "__main__":
    stack_usage_data = collect_stack_usage_info()
    report = generate_report(stack_usage_data)
    with open("stack_usage_report.txt", "w") as f:
        f.write(report)
