use crate::my_string;
#[derive(Debug, Clone)]
pub struct Decorator {
    pub name: String,
    pub arg: String,
}

impl Decorator {
    pub fn new(stmt: String) -> Option<Decorator> {
        // PIKA_C_MACRO_IF("MACRO_STMT") or PIKA_C_MACRO_IF('MACRO_STMT')
        let name = match my_string::get_first_token(&stmt, '(') {
            Some(name) => name,
            None => return None,
        };
        let arg_str = match my_string::cut(&stmt, '(', ')') {
            Some(arg) => arg,
            None => return None,
        };

        let arg = arg_str.replace("\"", "").replace("'", "").replace(" ", "");

        return Some(Decorator { name, arg });
    }

    pub fn gen_before(&self) -> String {
        if self.name == "PIKA_C_MACRO_IF" {
            return format!("#if {}\n", self.arg);
        }

        if self.name == "PIKA_C_MACRO_IFDEF" {
            return format!("#ifdef {}\n", self.arg);
        }

        return String::from("");
    }

    pub fn gen_after(&self) -> String {
        if self.name == "PIKA_C_MACRO_IF" {
            return String::from("#endif\n");
        }

        if self.name == "PIKA_C_MACRO_IFDEF" {
            return String::from("#endif\n");
        }

        return String::from("");
    }
}
