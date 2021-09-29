use crate::import_info::ImportInfo;
use crate::method_info::MethodInfo;
use crate::my_string;
use crate::object_info::ObjectInfo;
use crate::script::Script;
use std::collections::BTreeMap;
#[derive(Debug)]
pub struct ClassInfo {
    pub this_class_name: String,
    pub super_class_name: String,
    pub method_list: BTreeMap<String, MethodInfo>,
    pub object_list: BTreeMap<String, ObjectInfo>,
    pub import_list: BTreeMap<String, ImportInfo>,
    pub script_list: Script,
}

impl ClassInfo {
    pub fn add_file_profix(file_name: &String, class_name: &String) -> String {
        if file_name == "main" {
            return class_name.clone();
        } else if class_name == "BaseObj" || class_name == "TinyObj" {
            return class_name.clone();
        } else {
            return format!("{}_{}", file_name, class_name);
        }
    }

    pub fn new(file_name: &String, define: &String) -> Option<ClassInfo> {
        let define = define.strip_prefix("class ").unwrap().to_string();
        let define = define.replace(" ", "");
        let super_class_name = match my_string::cut(&define, '(', ')') {
            Some(s) => s,
            None => return None,
        };
        let super_class_name = match super_class_name.find(".") {
            None => ClassInfo::add_file_profix(&file_name, &super_class_name),
            Some(x) => super_class_name.replace(".", "_"),
        };
        let mut this_calss_name = match my_string::get_first_token(&define, '(') {
            Some(s) => s,
            None => return None,
        };
        this_calss_name = ClassInfo::add_file_profix(&file_name, &this_calss_name);
        let new_class_info = ClassInfo {
            this_class_name: this_calss_name,
            super_class_name: super_class_name,
            method_list: BTreeMap::new(),
            object_list: BTreeMap::new(),
            import_list: BTreeMap::new(),
            script_list: Script::new(),
        };
        return Some(new_class_info);
    }
    pub fn push_method(&mut self, method_define: String) {
        let method_info = match MethodInfo::new(&self.this_class_name, method_define) {
            Some(method) => method,
            None => return,
        };
        self.method_list
            .entry(method_info.name.clone())
            .or_insert(method_info);
    }
    pub fn push_import(&mut self, import_define: String, file_name: &String) {
        let import_info = match ImportInfo::new(&self.this_class_name, import_define, &file_name) {
            Some(import) => import,
            None => return,
        };
        self.import_list
            .entry(import_info.import_class_name.clone())
            .or_insert(import_info);
    }
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

    pub fn script_fn(&self) -> String {
        let mut script_fn = String::new();
        script_fn.push_str("PikaObj * pikaScriptInit(){\r\n");
        script_fn.push_str("    PikaObj * pikaMain = newRootObj(\"pikaMain\", New_PikaMain);\r\n");
        script_fn.push_str(&self.script_list.content);
        script_fn.push_str("    obj_run(pikaMain, \"\");\r\n");
        script_fn.push_str("    return pikaMain;\r\n");
        script_fn.push_str("}\r\n\r\n");
        return script_fn;
    }

    pub fn new_class_fn(&self) -> String {
        let mut new_class_fn = String::new();
        let new_class_fn_head = format!("{}{{\n", self.new_class_fn_name());
        new_class_fn.push_str(&new_class_fn_head);
        let derive = format!("    PikaObj *self = New_{}(args);\n", self.super_class_name);
        new_class_fn.push_str(&derive);
        for (_, import_info) in self.import_list.iter() {
            new_class_fn.push_str(&import_info.import_fn());
        }
        let mut if_only_import: bool = false;
        if self.this_class_name == "PikaMain" {
            if_only_import= true;
        }
        for (_, object_info) in self.object_list.iter() {
            new_class_fn.push_str(&object_info.new_object_fn(if_only_import));
        }

        for (_, method_info) in self.method_list.iter() {
            new_class_fn.push_str(&method_info.get_define());
        }

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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_analize() {
        assert_eq!(
            ClassInfo::new(
                &String::from("Pkg"),
                &String::from("class Test(SuperTest):")
            )
            .unwrap()
            .this_class_name,
            "Pkg_Test"
        );
        assert_eq!(
            ClassInfo::new(
                &String::from("Pkg"),
                &String::from("class Test(SuperTest):")
            )
            .unwrap()
            .super_class_name,
            "Pkg_SuperTest"
        );
    }
    #[test]
    fn test_push_method() {
        let mut class_info = ClassInfo::new(
            &String::from("Pkg"),
            &String::from("class Test(SuperTest):"),
        )
        .unwrap();
        class_info.push_method(String::from("def test(data: str)-> str:"));
        assert_eq!(
            class_info.method_list.get("test").unwrap().class_name,
            "Pkg_Test"
        );
        assert_eq!(class_info.method_list.get("test").unwrap().name, "test");
        assert_eq!(
            class_info
                .method_list
                .get("test")
                .as_ref()
                .unwrap()
                .return_type
                .as_ref()
                .unwrap()
                .to_string(),
            "str"
        );
        assert_eq!(
            class_info
                .method_list
                .get("test")
                .as_ref()
                .unwrap()
                .arg_list
                .as_ref()
                .unwrap()
                .to_string(),
            "data:str"
        );
    }
    #[test]
    fn test_push_object() {
        let mut class_info = ClassInfo::new(
            &String::from("Pkg"),
            &String::from("class Test(SuperTest):"),
        )
        .unwrap();
        class_info.push_object(String::from("testObj = TestObj()"), &"Pkg".to_string());
        assert_eq!(
            class_info.object_list.get("testObj").unwrap().class_name,
            "Pkg_Test"
        );
        assert_eq!(
            class_info.object_list.get("testObj").unwrap().name,
            "testObj"
        );
        assert_eq!(
            class_info
                .object_list
                .get("testObj")
                .unwrap()
                .import_class_name,
            "Pkg_TestObj"
        );
        assert_eq!(
            class_info.object_list.get("testObj").unwrap().name,
            "testObj"
        );
    }
    #[test]
    fn test_push_import() {
        let mut class_info = ClassInfo::new(
            &String::from("Pkg"),
            &String::from("class Test(SuperTest):"),
        )
        .unwrap();
        class_info.push_import(String::from("TestObj()"), &"Pkg".to_string());
        assert_eq!(
            class_info
                .import_list
                .get("Pkg_TestObj")
                .unwrap()
                .class_name,
            "Pkg_Test"
        );
        assert_eq!(
            class_info
                .import_list
                .get("Pkg_TestObj")
                .unwrap()
                .import_class_name,
            "Pkg_TestObj"
        );
    }
}
