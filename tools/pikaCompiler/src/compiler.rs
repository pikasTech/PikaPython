use crate::class_info::ClassInfo;
// use crate::my_string;
// use crate::script::Script;
use std::collections::BTreeMap;
use std::collections::LinkedList;
use std::fs;
use std::fs::File;
use std::io;
use std::io::prelude::*;
#[derive(Debug)]
pub struct Compiler {
    pub dist_path: String,
    pub source_path: String,
    pub class_list: BTreeMap<String, ClassInfo>,
    pub class_now_name: Option<String>,
    pub package_now_name: Option<String>,
    pub compiled_list: LinkedList<String>,
}

impl Compiler {
    pub fn new(source_path: String, dist_path: String) -> Compiler {
        let compiler = Compiler {
            dist_path: dist_path.clone(),
            source_path: source_path.clone(),
            class_now_name: None,
            class_list: BTreeMap::new(),
            package_now_name: None,
            compiled_list: LinkedList::new(),
        };
        return compiler;
    }
    pub fn analize_main_line(mut compiler: Compiler, line: &String) -> Compiler {
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

        if line.starts_with("import ") || line.starts_with("from ") {
            let tokens: Vec<&str> = line.split(' ').collect();
            let package_name = tokens[1];
            if package_name == "PikaObj" {
                return compiler;
            }
            let package_obj_define = format!("{} = {}()", package_name, package_name);
            class_now.push_object(package_obj_define, &file_name);
            class_now.script_list.add(&line);
            return Compiler::__do_analize_file(compiler, package_name.to_string(), true);
        }
        class_now.script_list.add(&line);
        return compiler;
    }

    fn open_file(path: String) -> io::Result<File> {
        for entry in fs::read_dir("./").expect("读取目录失败") {
            if let Ok(entry) = entry {
                let file = entry.path();
                let filename = file.to_str().unwrap().replace("./", "");
                if filename.to_string() == path {
                    return File::open(path);
                }
            }
        }
        return Err(std::io::Error::from(std::io::ErrorKind::NotFound));
    }

    pub fn __do_analize_file(mut self: Compiler, file_name: String, is_top_pkg: bool) -> Compiler {
        /* open file */
        let file: std::result::Result<std::fs::File, std::io::Error>;
        if file_name == "main" {
            file = Compiler::open_file(format!("{}main.py", self.source_path));
        } else {
            file = Compiler::open_file(format!("{}{}.pyi", self.source_path, file_name));
        }
        let mut file = match file {
            Ok(file) => file,
            Err(_) => {
                /* if *.py exits, not print warning */
                let file_py = Compiler::open_file(format!("{}{}.py", self.source_path, file_name));
                match file_py {
                    Ok(file) => file,
                    Err(_) => {
                        println!(
                            "    [warning]: file: '{}{}.pyi' or '{}{}.py' no found",
                            self.source_path, file_name, self.source_path, file_name
                        );
                        return self;
                    }
                };
                return self;
            }
        };
        let mut file_str = String::new();
        file.read_to_string(&mut file_str).unwrap();
        /* check if 'api' file */
        let lines: Vec<&str> = file_str.split('\n').collect();
        let mut is_api = false;
        if file_name == "main" {
            is_api = true;
        }
        for line in lines.iter() {
            if line.to_string().starts_with("#api") {
                is_api = true;
            }
        }
        if !is_api {
            return self;
        }

        /* check if compiled */
        if self.compiled_list.contains(&file_name) {
        } else if file_name == "main" {
            println!("    loading {}{}.py...", self.source_path, file_name);
        } else {
            println!("    binding {}{}.pyi...", self.source_path, file_name);
        }
        self.compiled_list.push_back(String::clone(&file_name));
        /* solve top package.
            About what is top package:
                Top package is the package imported by main.pyi,
                and can be used to new objcet in runtime.
                So the each classes of top package are loaded to flash.
            The top package is solved as an static object, and each calsses
                of the top package is solved as a function of the static
                object. To implament this, a [package]-api.c file is created
                to supply the class of static object.
        */
        if file_name != "main" && is_top_pkg {
            let pkg_define = format!("class {}(TinyObj):", &file_name);
            let package_now = match ClassInfo::new(&String::from(""), &pkg_define, true) {
                Some(s) => s,
                None => return self,
            };
            let package_name = package_now.this_class_name.clone();
            self.class_list
                .entry(package_name.clone())
                .or_insert(package_now);
            self.package_now_name = Some(package_name.clone());
        }
        let lines: Vec<&str> = file_str.split('\n').collect();
        /* analyze each line of pikascript-api.pyi */
        for line in lines.iter() {
            self = Compiler::analyze_line(self, line.to_string(), &file_name, is_top_pkg);
        }
        return self;
    }

    pub fn analyze_line(
        mut compiler: Compiler,
        line: String,
        file_name: &String,
        is_top_pkg: bool,
    ) -> Compiler {
        let line = line.replace("\r", "");
        if file_name == "main" {
            return Compiler::analize_main_line(compiler, &line);
        }

        if line.starts_with("import ") {
            let tokens: Vec<&str> = line.split(" ").collect();
            let file = tokens[1];
            return Compiler::__do_analize_file(compiler, file.to_string(), false);
        }

        if line.starts_with("#") {
            return compiler;
        }

        /* analize class stmt */
        if line.starts_with("class") {
            let class_now = match ClassInfo::new(&file_name, &line, false) {
                Some(s) => s,
                None => return compiler,
            };
            let class_name_without_file = class_now.this_class_name_without_file.clone();
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
                let package_new_object_method = format!("def {}()->any:", class_name_without_file);
                package_now.push_constructor(package_new_object_method);
            }

            return compiler;
        }

        /* analize def stmt */
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
