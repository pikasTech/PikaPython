use crate::class_info::ClassInfo;
use crate::decorator::Decorator;
// use crate::my_string;
// use crate::script::Script;
use fs_extra::dir::remove;
use std::collections::BTreeMap;
use std::collections::LinkedList;
use std::ffi::OsString;
use std::fs;
use std::fs::File;
use std::io;
use std::io::Read;
use std::path::Path;
use std::path::PathBuf;

enum PackageType {
    CPackageTop,
    CPackageInner,
    PyPackageTop,
    PyPackageInner,
}

#[derive(Debug)]
pub struct Compiler {
    pub dist_path: String,
    pub source_path: String,
    pub class_list: BTreeMap<String, ClassInfo>,
    pub class_name_now: Option<String>,
    pub package_name_now: Option<String>,
    pub compiled_list: LinkedList<String>,
    pub decorator_list_now: Vec<Decorator>,
}

impl Compiler {
    pub fn new(source_path: String, dist_path: String) -> Compiler {
        let compiler = Compiler {
            dist_path: dist_path.clone(),
            source_path: source_path.clone(),
            class_name_now: None,
            class_list: BTreeMap::new(),
            package_name_now: None,
            compiled_list: LinkedList::new(),
            decorator_list_now: Vec::new(),
        };
        return compiler;
    }

    pub fn clean_path(&mut self) {
        /* create path if not exist */
        if !fs::metadata(&self.dist_path).is_ok() {
            fs::create_dir_all(&self.dist_path).expect("Failed to create directory");
        } else {
            /* rename the path to self.dist_path+'-old' */
            let trimmed_path = self.dist_path.trim_end_matches('/');
            let old_path = format!("{}-old", trimmed_path);
            if fs::metadata(&old_path).is_ok() {
                remove(&old_path).expect("Failed to remove old directory");
                // delete old path if it exists
            }
            fs::rename(&self.dist_path, &old_path).expect("Failed to rename directory"); // rename current path
            fs::create_dir_all(&self.dist_path).expect("Failed to create directory");
            // create new path
        }
    }

    pub fn reuse_old_file(&mut self) {
        let old_path = self.get_old_path();
        let new_path = Path::new(&self.dist_path);

        if self.paths_are_valid(&old_path, &new_path) {
            self.sync_from_old_to_new(&old_path, &new_path);
            self.sync_from_new_to_old(&new_path, &old_path);
            self.finalize_directory_changes(&new_path, &old_path);
        }
    }

    // Returns the path to the "old" directory.
    fn get_old_path(&self) -> PathBuf {
        format!("{}-old", self.dist_path.trim_end_matches('/')).into()
    }

    // Check if both the old and new paths are valid directories.
    fn paths_are_valid(&self, old_path: &Path, new_path: &Path) -> bool {
        old_path.exists() && old_path.is_dir() && new_path.exists() && new_path.is_dir()
    }

    // Synchronize files from old directory to new directory.
    fn sync_from_old_to_new(&self, old_path: &Path, new_path: &Path) {
        let entries = fs::read_dir(&old_path);
        if let Ok(entries_iter) = entries {
            for entry in entries_iter {
                let entry = entry.expect("Failed to read entry in old directory");
                let file_name = entry.file_name();
                let new_file_path = new_path.join(&file_name);

                if new_file_path.exists() {
                    self.handle_existing_file_in_new(&file_name, old_path, new_path);
                } else {
                    self.handle_missing_file_in_new(&file_name, old_path);
                }
            }
        }
    }

    // If a file from the old directory exists in the new directory, we compare and act accordingly.
    fn handle_existing_file_in_new(&self, file_name: &OsString, old_path: &Path, new_path: &Path) {
        let old_file_path = old_path.join(&file_name);
        let new_file_path = new_path.join(&file_name);

        let old_contents = fs::read(&old_file_path).expect("Failed to read old file");
        let new_contents = fs::read(&new_file_path).expect("Failed to read new file");

        if old_contents != new_contents {
            fs::copy(&new_file_path, &old_file_path)
                .expect("Failed to copy new file to old directory");
        }
    }

    // If a file from the old directory is missing in the new directory, we delete it from the old directory.
    fn handle_missing_file_in_new(&self, file_name: &OsString, old_path: &Path) {
        fs::remove_file(old_path.join(&file_name))
            .expect("Failed to remove file from old directory");
    }

    // Synchronize files from new directory to old directory.
    fn sync_from_new_to_old(&self, new_path: &Path, old_path: &Path) {
        let entries = fs::read_dir(&new_path);
        if let Ok(entries_iter) = entries {
            for entry in entries_iter {
                let entry = entry.expect("Failed to read entry in new directory");
                let file_name = entry.file_name();

                if !old_path.join(&file_name).exists() {
                    fs::copy(new_path.join(&file_name), old_path.join(&file_name))
                        .expect("Failed to copy new file to old directory");
                }
            }
        }
    }

    // Remove the new directory and rename the old directory.
    fn finalize_directory_changes(&self, new_path: &Path, old_path: &Path) {
        fs::remove_dir_all(&new_path).expect("Failed to remove new directory");
        fs::rename(&old_path, &new_path).expect("Failed to rename old directory to new directory");
    }

    fn get_or_create_main_class(&mut self) -> &mut ClassInfo {
        if !self.class_list.contains_key(&"PikaMain".to_string()) {
            let new_class_info = ClassInfo::new(
                &"main".to_string(),
                &"class PikaMain(PikaStdLib.SysObj):".to_string(),
                false,
            )
            .expect("Failed to create PikaMain class");
            self.class_list
                .insert("PikaMain".to_string(), new_class_info);
        }

        self.class_name_now = Some("PikaMain".to_string());

        self.class_list.get_mut(&"PikaMain".to_string()).unwrap()
    }

    pub fn analyse_py_line(mut compiler: Compiler, line: &String, is_top_pkg: bool) -> Compiler {
        /* get class main or create one */
        let class_main = compiler.get_or_create_main_class();

        if line.starts_with("import ") {
            let tokens: Vec<&str> = line.split(" as ").collect();
            let package_list = tokens[0];
            let package_list = package_list.replace("import ", "");
            let package_list = package_list.replace(" ", "");
            // get token 1 to
            if package_list == "PikaObj" {
                return compiler;
            }

            if is_top_pkg {
                /* add to script */
                class_main.script_list.add(&line);
            }

            let package_items: Vec<&str> = package_list.split(',').collect();
            for item in package_items {
                compiler = Compiler::import_module_ex(compiler, item.to_string(), true);
            }
            return compiler;
        }

        if line.starts_with("from ") {
            let tokens: Vec<&str> = line.split(' ').collect();
            // get token 1 to
            let package_name = tokens[1];
            if package_name == "PikaObj" {
                return compiler;
            }

            if is_top_pkg {
                /* add to script */
                class_main.script_list.add(&line);
            }

            return Compiler::import_module_ex(compiler, package_name.to_string(), true);
        }

        if is_top_pkg {
            class_main.script_list.add(&line);
        }
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

    pub fn analyse_py_package_main(self: Compiler, file_name: String) -> Compiler {
        return self.__do_analyse_file(file_name, PackageType::PyPackageTop);
    }

    pub fn analyse_py_package_inner(self: Compiler, file_name: String) -> Compiler {
        return self.__do_analyse_file(file_name, PackageType::PyPackageInner);
    }

    pub fn analyse_c_package_top(self: Compiler, file_name: String) -> Compiler {
        return self.__do_analyse_file(file_name, PackageType::CPackageTop);
    }

    pub fn analyse_c_package_inner(self: Compiler, file_name: String) -> Compiler {
        return self.__do_analyse_file(file_name, PackageType::CPackageInner);
    }

    // Try to open a file and return whether it was opened successfully.
    fn try_open_file(&self, file_name: &str, suffix: &str) -> bool {
        let file_path = format!("{}{}.{}", self.source_path, file_name, suffix);
        Compiler::open_file(file_path).is_ok()
    }

    // Logic to handle pyi files.
    fn handle_pyi_file(mut self, file_name: &str) -> Compiler {
        if let Some(class_now) = self.class_list.get_mut(&"PikaMain".to_string()) {
            // Create a constructor for PikaMain.
            let package_obj_define = format!("{} = {}()", file_name, file_name);
            class_now.push_object(package_obj_define, &"main".to_string());
            return Compiler::analyse_c_package_top(self, file_name.to_string());
        }
        return self;
    }

    pub fn import_module_ex(self, file_name: String, from_scan: bool) -> Compiler {
        // Check for py.o file.
        if self.try_open_file(&file_name, "py.o") {
            println!("    found {}{}.py.o...", self.source_path, file_name);
            return self;
        }

        // Check for pyi file.
        if self.try_open_file(&file_name, "pyi") {
            if from_scan {
                println!("    binding {}{}.{}...", self.source_path, file_name, "pyi");
            } else {
                println!("  binding {}{}.{}...", self.source_path, file_name, "pyi");
            }
            return self.handle_pyi_file(&file_name);
        }

        // Handle py import for py file.
        self.analyse_py_package_inner(file_name)
    }

    pub fn import_module(self, file_name: String) -> Compiler {
        return self.import_module_ex(file_name, false);
    }

    fn __do_analyse_file(mut self: Compiler, file_name: String, pkg_type: PackageType) -> Compiler {
        let is_top_c_pkg = match pkg_type {
            PackageType::CPackageTop => true,
            _ => false,
        };

        let is_top_py_pkg: bool = match pkg_type {
            PackageType::PyPackageTop => true,
            _ => false,
        };

        let suffix = match pkg_type {
            PackageType::CPackageTop | PackageType::CPackageInner => "pyi",
            PackageType::PyPackageTop | PackageType::PyPackageInner => "py",
        };
        /* open file */
        let file: std::result::Result<std::fs::File, std::io::Error>;
        file = Compiler::open_file(format!("{}{}.{}", self.source_path, file_name, suffix));

        /*
            If the .pyi no exist, check if the .py exist
                the .py files are solved by byteCodeGen, so if
                there is .py files, binder need not print warning.
        */
        let mut file = match file {
            Ok(file) => file,
            Err(_) => {
                if suffix == "pyi" {
                    /* if .pyi no exist, check .py exist */
                    return self.analyse_py_package_inner(file_name.clone());
                }
                /* .py no exist, error */
                println!(
                    "    [warning]: file: '{}{}.pyi', '{}{}.py' or '{}{}.py.o' no found",
                    self.source_path,
                    file_name,
                    self.source_path,
                    file_name,
                    self.source_path,
                    file_name
                );
                return self;
            }
        };

        let mut file_str = String::new();
        file.read_to_string(&mut file_str).unwrap();

        if suffix == "pyi" {
            /*
                Push top C package to PikaMain.
                Solve the package as a class
                    [Example]:
                        class PikaStdDevice(TinyObj):
            */
            let pkg_define = format!("class {}(TinyObj):", &file_name);
            let mut package_now = match ClassInfo::new(&String::from(""), &pkg_define, true) {
                Some(s) => s,
                None => return self,
            };
            let package_name = file_name.clone();
            // println!("package_name: {}", package_name);
            if is_top_c_pkg {
                self.package_name_now = Some(package_name.clone());
                package_now.is_top = true;
                // println!("set top package: {}", package_name);
            }
            match self.class_list.entry(package_name.clone()) {
                std::collections::btree_map::Entry::Occupied(mut entry) => {
                    // 如果 `package_now.is_top == true`，将 `class_list` 中对应项的 `is_top` 设为 `true`
                    if package_now.is_top {
                        let class_info = entry.get_mut();
                        // 需要确保 `ClassInfo` 结构体中存在 `is_top` 字段
                        class_info.is_top = true;
                    }
                }
                std::collections::btree_map::Entry::Vacant(entry) => {
                    entry.insert(package_now);
                }
            }
        }

        /* return when compiled */
        if !self.compiled_list.contains(&file_name) {
            /* print info */
            match suffix {
                "py" => {
                    println!("  scanning {}{}.{}...", self.source_path, file_name, suffix);
                }
                "pyi" => {}
                _ => {}
            }
        } else {
            /* not scan *.py again */
            match suffix {
                "py" => return self,
                _ => {}
            }
        }

        /* push to compiled list, only compile once */
        self.compiled_list.push_back(String::clone(&file_name));
        /* solve top package.
            About what is top package:
                Top package is the package imported by main.py directly,
                and can be used to create objcet in runtime.
                So the each classes of top package are loaded to flash.
            The top package is solved as an static object, and each calsses
                of the top package is solved as a function of the static
                object. To implament this, a [package]-api.c file is created
                to supply the class of static object.
        */

        let lines: Vec<&str> = file_str.split('\n').collect();
        let mut last_line = String::from("");
        /* analyse each line */
        for line in lines.iter() {
            let mut line = line.replace("\r", "");
            /* replace \t with 4 spaces */
            line = line.replace("\t", "    ");
            if line.contains("(") && !line.contains(")") {
                last_line = line.clone();
                continue;
            }
            if last_line != "" {
                if line.contains(")") {
                    line = format!("{}{}", last_line, line);
                    last_line = String::from("");
                } else {
                    last_line = format!("{}{}", last_line, line);
                    continue;
                }
            }
            self = match pkg_type {
                PackageType::CPackageTop | PackageType::CPackageInner => {
                    Compiler::analyse_pyi_line(self, line.to_string(), &file_name, is_top_c_pkg)
                }
                PackageType::PyPackageTop | PackageType::PyPackageInner => {
                    Compiler::analyse_py_line(self, &line, is_top_py_pkg)
                }
            };
        }
        return self;
    }

    pub fn analyse_pyi_line(
        mut compiler: Compiler,
        line: String,
        file_name: &String,
        is_top_pkg: bool,
    ) -> Compiler {
        if line.starts_with("import ") {
            let tokens: Vec<&str> = line.split(" ").collect();
            let file = tokens[1];
            /* cmodule cannot import pymodule */
            return Compiler::analyse_c_package_inner(compiler, file.to_string());
        }

        if line.trim_start().starts_with("#") {
            return compiler;
        }

        if line.starts_with("@") || line.starts_with("    @") {
            let stmt = line.replace("@", "").replace(" ", "");
            let decorator_this = match Decorator::new(stmt) {
                Some(s) => s,
                None => return compiler,
            };
            compiler.decorator_list_now.push(decorator_this);
            return compiler;
        }

        /* analyse class stmt */
        if line.starts_with("class") {
            /* create a new class */
            let class_now = match ClassInfo::new(&file_name, &line, false) {
                Some(s) => s,
                None => return compiler,
            };
            let class_name_without_file = class_now.this_class_name_without_file.clone();
            let class_name = class_now.this_class_name.clone();
            /* regist this class to the class list of compiler */
            compiler
                .class_list
                .entry(class_name.clone())
                .or_insert(class_now);
            compiler.class_name_now = Some(class_name.clone());

            /* the class in to package can be import in runtime */
            if is_top_pkg {
                /*
                    Solve the class as method of top package
                        the package is a class in main.py and the class is a method of the class in main.py.
                */
                let package_now_name = match compiler.package_name_now.clone() {
                    Some(s) => s,
                    None => return compiler,
                };
                let package_now = compiler.class_list.get_mut(&package_now_name).unwrap();
                let package_new_object_method = format!("def {}()->any:", class_name_without_file);
                package_now.push_constructor(
                    package_new_object_method,
                    compiler.decorator_list_now.clone(),
                );
                compiler.decorator_list_now.clear();
            }

            return compiler;
        }

        /* analyse function define */
        if line.starts_with("def ") {
            let package_now_name = file_name.clone();
            let package_now = match compiler.class_list.get_mut(&package_now_name) {
                Some(s) => s,
                None => return compiler,
            };
            package_now.push_method(line, compiler.decorator_list_now.clone());
            compiler.decorator_list_now.clear();
            return compiler;
        }

        /* analyse def stmt inner class */
        if line.starts_with("    def ") {
            let line = line.strip_prefix("    ").unwrap().to_string();
            let class_now = compiler
                .class_list
                .get_mut(&compiler.class_name_now.clone().unwrap())
                .unwrap();
            class_now.push_method(line, compiler.decorator_list_now.clone());
            compiler.decorator_list_now.clear();
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
                .get_mut(&compiler.class_name_now.clone().unwrap())
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
    fn test_analyse() {
        let compiler = Compiler::new(String::from(""), String::from(""));
        let compiler = Compiler::analyse_pyi_line(
            compiler,
            String::from("class Test(SuperTest):"),
            &"Pkg".to_string(),
            false,
        );
        let compiler = Compiler::analyse_pyi_line(
            compiler,
            String::from("    def test()"),
            &"Pkg".to_string(),
            false,
        );
        let compiler = Compiler::analyse_pyi_line(
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
