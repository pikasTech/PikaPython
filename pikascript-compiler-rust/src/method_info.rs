use crate::arg_list::ArgList;
use crate::my_string;
use crate::py_type::PyType;

#[derive(Debug)]
pub struct MethodInfo {
    pub class_name: String,
    pub name: String,
    pub arg_list: Option<ArgList>,
    pub return_type: Option<PyType>,
}

impl MethodInfo {
    pub fn new(class_name: &String, input_define: String) -> Option<MethodInfo> {
        let define = match input_define.strip_prefix("def ") {
            Some(define) => define.to_string(),
            None => return None,
        };
        let define = define.replace(" ", "");
        let name = match my_string::get_first_token(&define, '(') {
            Some(token) => token,
            None => return None,
        };
        let arg_list = my_string::cut(&define, '(', ')');
        let return_type = match my_string::cut(&define, '>', ':') {
            Some(py_type) => Some(PyType::new(&py_type)),
            None => None,
        };
        let method_info = MethodInfo {
            name: name,
            arg_list: ArgList::new(&arg_list),
            return_type: return_type,
            class_name: class_name.clone(),
        };
        return Some(method_info);
    }
    pub fn get_define(&self) -> String {
        let return_token = match &self.return_type {
            Some(s) => format!("->{}", s.to_string()),
            None => String::from(""),
        };
        let arg_list = match &self.arg_list {
            Some(t) => t.to_string(),
            None => String::from(""),
        };
        let define = format!(
            "    class_defineMethod(self, \"{}({}){}\", {}_{}Method);\n",
            self.name, arg_list, return_token, self.class_name, self.name
        );
        return define;
    }
    pub fn method_api_name(&self) -> String {
        return format!(
            "void {}_{}Method(PikaObj *self, Args *args)",
            self.class_name, self.name
        );
    }
    pub fn method_impl_declear(&self) -> String {
        let return_type_in_c = match self.return_type.as_ref() {
            Some(x) => x.to_c_type(),
            None => String::from("void"),
        };
        let arg_list_in_c = match self.arg_list.as_ref() {
            Some(x) => format!(", {}", x.to_c()),
            None => String::from(""),
        };
        return format!(
            "{} {}_{}(PikaObj *self{});\n",
            return_type_in_c, self.class_name, self.name, arg_list_in_c,
        );
    }
    pub fn method_fn_impl(&self) -> String {
        let mut method_fn_impl = "".to_string();
        let method_fn_name = format!("{}{{\n", self.method_api_name());
        let get_local_args = match &self.arg_list {
            Some(x) => x.get_local_args(),
            None => "".to_string(),
        };
        let return_impl = match &self.return_type {
            Some(x) => format!("    {}(args, res);\n", x.return_fn()),
            None => "".to_string(),
        };
        let return_type_in_c = match &self.return_type {
            Some(x) => format!("{} res = ", x.to_c_type()),
            None => "".to_string(),
        };
        let call_arg_list = match &self.arg_list {
            Some(x) => format!(", {}", x.call_arg_list()),
            None => "".to_string(),
        };
        let call_method = format!(
            "    {}{}_{}(self{});\n",
            return_type_in_c, self.class_name, self.name, call_arg_list
        );
        method_fn_impl.push_str(&method_fn_name);
        method_fn_impl.push_str(&get_local_args);
        method_fn_impl.push_str(&call_method);
        method_fn_impl.push_str(&return_impl);
        method_fn_impl.push_str("}\n\n");
        return method_fn_impl;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_get_local_declear() {
        let method_info = MethodInfo::new(
            &String::from("Test"),
            String::from("def test(test:str, test2:int)->str:"),
        );
        let define = method_info.as_ref().unwrap().method_impl_declear();
        let method_fn_impl = method_info.as_ref().unwrap().method_fn_impl();
        assert_eq!(define, "char * Test_test(PikaObj *self, char * test, int test2);\n");
        assert_eq!(
            method_fn_impl,
            "void Test_testMethod(PikaObj *self, Args *args){\n    char * test = args_getStr(args, \"test\");\n    int test2 = args_getInt(args, \"test2\");\n    char * res = Test_test(self, test, test2);\n    method_returnStr(args, res);\n}\n\n"
        );
    }

    #[test]
    fn test_analize() {
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test:str)->str:")
            )
            .unwrap()
            .name,
            String::from("test")
        );
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test:str)->str:")
            )
            .unwrap()
            .return_type
            .unwrap()
            .to_string(),
            String::from("str")
        );
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test:str)->str:")
            )
            .unwrap()
            .arg_list
            .unwrap()
            .to_string(),
            String::from("test:str")
        );
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test: str) ->str:")
            )
            .unwrap()
            .name,
            String::from("test")
        );
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test: str) ->str:")
            )
            .unwrap()
            .return_type
            .unwrap()
            .to_string(),
            String::from("str")
        );
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test: str) ->str:")
            )
            .unwrap()
            .arg_list
            .unwrap()
            .to_string(),
            String::from("test:str")
        );
        assert_eq!(
            MethodInfo::new(
                &String::from("Test"),
                String::from("def test(test: str, test2: int) ->str:")
            )
            .unwrap()
            .arg_list
            .unwrap()
            .to_string(),
            String::from("test:str,test2:int")
        );
    }
    #[test]
    fn test_get_define() {
        let method_info = MethodInfo::new(
            &String::from("Test"),
            String::from("def test(test:str, test2:int)->str:"),
        );
        let define = method_info.unwrap().get_define();
        assert_eq!(define, String::from("    class_defineMethod(self, \"test(test:str,test2:int)->str\", Test_testMethod);\n"));
    }
    #[test]
    fn test_get_define_no_return() {
        let method_info = MethodInfo::new(
            &String::from("Test"),
            String::from("def test(test:str, test2:int):"),
        );
        let define = method_info.unwrap().get_define();
        assert_eq!(
            define,
            String::from(
                "    class_defineMethod(self, \"test(test:str,test2:int)\", Test_testMethod);\n"
            )
        );
    }
    #[test]
    fn test_get_define_no_return_no_arg_list() {
        let method_info =
            MethodInfo::new(&String::from("Test"), String::from("def test():")).unwrap();
        let define = method_info.get_define();
        let method_fn_name = method_info.method_api_name();
        assert_eq!(
            define,
            String::from("    class_defineMethod(self, \"test()\", Test_testMethod);\n")
        );
        assert_eq!(
            method_fn_name,
            String::from("void Test_testMethod(PikaObj *self, Args *args)")
        );
    }
}
