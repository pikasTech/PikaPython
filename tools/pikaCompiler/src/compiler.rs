use crate::class_info::ClassInfo;
use crate::my_string;
use crate::script::Script;
use std::collections::BTreeMap;
use std::fs::File;
use std::io::prelude::*;
#[derive(Debug)]
pub struct Compiler {
    pub dist_path: String,
    pub source_path: String,
    pub class_list: BTreeMap<String, ClassInfo>,
    pub class_now_name: Option<String>,
    pub package_now_name: Option<String>,
}

impl Compiler {
    pub fn new(source_path: String, dist_path: String) -> Compiler {
        let compiler = Compiler {
            dist_path: dist_path.clone(),
            source_path: source_path.clone(),
            class_now_name: None,
            class_list: BTreeMap::new(),
            package_now_name: None,
        };
        return compiler;
    }
    pub fn analyze_main_line(mut compiler: Compiler, line: &String) -> Compiler {
        let file_name = "main".to_string();
        let class_name = "PikaMain".to_string();
        /* get class now or create one */
        let class_now = match compiler.class_list.get_mut(&"PikaMain".to_string()) {
            Some(class_now) => class_now,
            None => compiler.class_list.entry(class_name.clone()).or_insert(
                ClassInfo::new(
                    &file_name,
                    &"class PikaMain(PikaStdLib.SysObj):".to_string(),
                    false,
                )
                .unwrap(),
            ),
        };
        compiler.class_now_name = Some(class_name.clone());

        if line.starts_with("from ") {
            return compiler;
        }
        if line.starts_with("import ") {
            let package_name = my_string::get_last_token(&line, ' ').unwrap();
            let package_obj_define = format!("{} = {}()", package_name, package_name);
            class_now.push_object(package_obj_define, &file_name);
            return Compiler::analyze_file(compiler, package_name.to_string(), true);
        }
        class_now.script_list.add(&line);
        return compiler;
    }

    pub fn analyze_file(mut compiler: Compiler, file_name: String, is_top_pkg: bool) -> Compiler {
        println!(
            "(PikaScript) compiling {}{}.py...",
            compiler.source_path, file_name
        );
        let file = File::open(format!("{}{}.py", compiler.source_path, file_name));
        let mut file = match file {
            Ok(file) => file,
            Err(_) => panic!("[error]: file: '{}{}.py' no found", compiler.source_path, file_name),
        };
        /* solve package as top class */
        if file_name != "main" && is_top_pkg {
            let pkg_define = format!("class {}(TinyObj):", &file_name);
            let pacakge_now = match ClassInfo::new(&String::from(""), &pkg_define, true) {
                Some(s) => s,
                None => return compiler,
            };
            let package_name = pacakge_now.this_class_name.clone();
            compiler
                .class_list
                .entry(package_name.clone())
                .or_insert(pacakge_now);
            compiler.package_now_name = Some(package_name.clone());
        }
        /* solve lines in file */
        let mut file_str = String::new();
        file.read_to_string(&mut file_str).unwrap();
        let lines: Vec<&str> = file_str.split('\n').collect();
        /* analyze each line of pikascript-api.py */
        for line in lines.iter() {
            compiler = Compiler::analyze_line(compiler, line.to_string(), &file_name, is_top_pkg);
        }
        return compiler;
    }

    pub fn analyze_line(
        mut compiler: Compiler,
        line: String,
        file_name: &String,
        is_top_pkg: bool,
    ) -> Compiler {
        let line = line.replace("\r", "");
        if file_name == "main" {
            return Compiler::analyze_main_line(compiler, &line);
        }

        if line.starts_with("import ") {
            let tokens: Vec<&str> = line.split(" ").collect();
            let file = tokens[1];
            return Compiler::analyze_file(compiler, file.to_string(), false);
        }

        if line.starts_with("#") {
            return compiler;
        }

        if line.starts_with("class") {
            let class_now = match ClassInfo::new(&file_name, &line, false) {
                Some(s) => s,
                None => return compiler,
            };
            let class_name = class_now.this_class_name.clone();
            compiler
                .class_list
                .entry(class_name.clone())
                .or_insert(class_now);
            compiler.class_now_name = Some(class_name.clone());

            if is_top_pkg {
                /* solve the class as method of top package*/
                let package_now_name = match compiler.package_now_name.clone() {
                    Some(s) => s,
                    None => return compiler,
                };
                let package_now = compiler.class_list.get_mut(&package_now_name).unwrap();
                let class_name_without_file = match my_string::get_last_token(&class_name, '_') {
                    Some(s) => s,
                    None => return compiler,
                };
                let package_new_object_method = format!("def {}()->any:", class_name_without_file);
                package_now.push_method(package_new_object_method);
            }

            return compiler;
        }

        if line.starts_with("    def ") {
            let line = line.strip_prefix("    ").unwrap().to_string();
            let class_now = compiler
                .class_list
                .get_mut(&compiler.class_now_name.clone().unwrap())
                .unwrap();
            class_now.push_method(line);
            return compiler;
        }

        if line.starts_with("    ")
            && line.contains("(")
            && line.contains(")")
            && line.contains("=")
        {
            let line = line.strip_prefix("    ").unwrap().to_string();
            let class_now = compiler
                .class_list
                .get_mut(&compiler.class_now_name.clone().unwrap())
                .unwrap();
            class_now.push_object(line, &file_name);
            return compiler;
        }

        return compiler;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_analyze() {
        let compiler = Compiler::new(String::from(""), String::from(""));
        let compiler = Compiler::analyze_line(
            compiler,
            String::from("class Test(SuperTest):"),
            &"Pkg".to_string(),
            false,
        );
        let compiler = Compiler::analyze_line(
            compiler,
            String::from("    def test()"),
            &"Pkg".to_string(),
            false,
        );
        let compiler = Compiler::analyze_line(
            compiler,
            String::from("    testObj = TestObj()"),
            &"Pkg".to_string(),
            false,
        );

        let class_info = compiler.class_list.get("Pkg_Test").unwrap();
        let method_info = class_info.method_list.get("test").unwrap();
        let object_info = class_info.object_list.get("testObj").unwrap();
        assert_eq!(class_info.this_class_name, "Pkg_Test");
        assert_eq!(class_info.super_class_name, "Pkg_SuperTest");
        assert_eq!(method_info.name, "test");
        assert_eq!(method_info.class_name, "Pkg_Test");
        assert_eq!(object_info.name, "testObj");
        assert_eq!(object_info.class_name, "Pkg_Test");
    }
}
