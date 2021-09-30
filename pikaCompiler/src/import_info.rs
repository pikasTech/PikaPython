use crate::my_string;
use crate::class_info;
#[derive(Debug)]
pub struct ImportInfo {
    pub class_name: String,
    pub import_class_name: String,
}

impl ImportInfo {
    pub fn new(
        class_name: &String,
        input_define: String,
        file_name: &String,
    ) -> Option<ImportInfo> {
        let define = input_define.replace(" ", "");
        let mut import_class_name = match my_string::get_first_token(&define, '(') {
            Some(token) => token,
            None => return None,
        };
        import_class_name = match import_class_name.find(".") {
            None => class_info::ClassInfo::add_file_profix(&file_name, &import_class_name),
            Some(x) => import_class_name.replace(".", "_"),
        };
        import_class_name = import_class_name.replace(".", "_");
        return Some(ImportInfo {
            class_name: class_name.clone(),
            import_class_name: import_class_name,
        });
    }
    pub fn import_fn(&self) -> String {
        return format!(
            "    obj_import(self, \"{}\", New_{});\n",
            self.import_class_name, self.import_class_name
        );
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_import_info() {
        assert_eq!(
            ImportInfo::new(
                &String::from("Test"),
                String::from("ImportTest()"),
                &"Pkg".to_string()
            )
            .unwrap()
            .import_class_name,
            String::from("Pkg_ImportTest")
        );
        assert_eq!(
            ImportInfo::new(
                &String::from("Test"),
                String::from("ImportTest()"),
                &"Pkg".to_string()
            )
            .unwrap()
            .class_name,
            String::from("Test")
        );
    }
}
