use crate::class_info::ClassInfo;
use crate::my_string;
#[derive(Debug)]
pub struct Script {
    pub content: String,
}

impl Script {
    pub fn new() -> Script {
        return Script {
            content: String::new(),
        };
    }
    pub fn add(&mut self, content: &String) {
        self.content.push_str(&Script::obj_run(content));
    }
    pub fn assert(class_info: &ClassInfo, content: &String) -> bool {
        let cmd = my_string::cut(content, '=', '(').unwrap();
        let cmd = cmd.replace(" ", "");
        let called_first_object = match my_string::get_first_token(&cmd, '.') {
            Some(token) => token,
            None => cmd,
        };
        for (_, obj_info) in class_info.object_list.iter() {
            let obj_name = obj_info.name.clone();
            if called_first_object == obj_name {
                return true;
            }
        }
        return false;
    }
    pub fn obj_run(cmd: &String) -> String {
        return format!("            \"{}\\n\"\n", cmd);
    }
}
