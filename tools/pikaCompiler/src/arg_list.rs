use crate::my_string;
use crate::py_arg::PyArg;
#[derive(Debug)]
pub struct ArgList {
    py_arg_list: String,
    list: Vec<PyArg>,
}

impl ArgList {
    pub fn to_string(&self) -> String {
        return self.py_arg_list.clone();
    }

    pub fn new(py_arg_list: &Option<String>) -> Option<ArgList> {
        let py_arg_list = match py_arg_list {
            Some(x) => x,
            None => return None,
        };
        /* remove space */
        let py_arg_list = py_arg_list.replace(" ", "");
        /* remove no type arg */
        let py_arg_list_vecotr: Vec<&str> = py_arg_list.split(",").collect();
        let mut py_arg_list = String::from("");
        for arg_define in py_arg_list_vecotr.iter() {
            let arg_define = String::from(*arg_define);
            if arg_define.starts_with("*") {
                /* variable parameters tuple */
                py_arg_list.push_str(&arg_define);
                py_arg_list.push_str(",");
            } else if arg_define.contains(":") {
                /* typed arg */
                py_arg_list.push_str(&arg_define);
                py_arg_list.push_str(",");
            }
        }
        if py_arg_list.contains(",") {
            /* remove the last ',' */
            py_arg_list.remove(py_arg_list.len() - 1);
        } else {
            return None;
        }

        /* push py_arg_list */
        let mut arg_list = ArgList {
            py_arg_list: py_arg_list.clone(),
            list: Vec::new(),
        };

        /* splite each arg */
        let py_arg_list_vecotr: Vec<&str> = py_arg_list.split(",").collect();
        for arg_define in py_arg_list_vecotr.iter() {
            /* get arg name */
            let mut arg_name = match my_string::get_first_token(&arg_define.to_string(), ':') {
                Some(name) => name,
                /* if not get ':', get the name */
                None => String::from(arg_define.to_string()),
            };
            if arg_name.starts_with("*") {
                /* is the tuple variable parameter */
                arg_name = arg_name.strip_prefix("*").unwrap().to_string();
            }
            /* get type name */
            let type_name = match my_string::get_last_token(&arg_define.to_string(), ':') {
                Some(name) => name,
                /* if not get ':', ignore the arg */
                None => {
                    if arg_define.starts_with("*") {
                        /* is the tuple variable parameter */
                        String::from("@tupleVarPar")
                    } else {
                        String::from("")
                    }
                }
            };
            arg_list.list.push(PyArg::new(&arg_name, &type_name));
        }
        return Some(arg_list);
    }
    pub fn to_c(&self) -> String {
        let mut arg_list_in_c = String::from("");
        for (i, py_arg) in self.list.iter().enumerate() {
            let arg_name = py_arg.name();
            let type_name_in_c = py_arg.c_type();
            arg_list_in_c.push_str(&type_name_in_c);
            arg_list_in_c.push_str(" ");
            arg_list_in_c.push_str(&arg_name);
            if i < self.list.len() - 1 {
                arg_list_in_c.push_str(", ");
            }
        }
        return arg_list_in_c;
    }
    pub fn call_arg_list(&self) -> String {
        let mut call_arg_list = "".to_string();
        for (i, py_arg) in self.list.iter().enumerate() {
            let arg_name = py_arg.name();
            call_arg_list.push_str(&arg_name);
            if i < self.list.len() - 1 {
                call_arg_list.push_str(", ");
            }
        }
        return call_arg_list;
    }
    pub fn get_local_args(&self) -> String {
        let mut get_local_args = "".to_string();
        for py_arg in self.list.iter() {
            get_local_args.push_str(&py_arg.get_local_arg());
        }
        return get_local_args;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_arg_list() {
        let arg_list =
            ArgList::new(&Some(String::from("arg1: str, arg2: int, arg3: FILE"))).unwrap();
        let arg_list_in_c = arg_list.to_c();
        let call_arg_list = arg_list.call_arg_list();
        let get_local_args = arg_list.get_local_args();
        assert_eq! {arg_list_in_c,"char * arg1, int arg2, void * arg3"};
        assert_eq! {call_arg_list,"arg1, arg2, arg3"};
        assert_eq! {get_local_args,"    char * arg1 = args_getStr(args, \"arg1\");\n    int arg2 = args_getInt(args, \"arg2\");\n    void * arg3 = args_getPtr(args, \"arg3\");\n"};
    }
    #[test]
    fn test_arg_list_one_arg() {
        let arg_list = ArgList::new(&Some(String::from("argtest: str"))).unwrap();
        let arg_list_in_c = arg_list.to_c();
        assert_eq! {arg_list_in_c,"char * argtest"};
    }
}
