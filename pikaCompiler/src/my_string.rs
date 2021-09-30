pub fn cut(string: &String, start_char: char, end_char: char) -> Option<String> {
    let start_index = match string.find(start_char) {
        Some(i) => i,
        None => return None,
    };
    let end_index = match string.rfind(end_char) {
        Some(i) => i,
        None => return None,
    };
    if start_index + 1 == end_index {
        return None;
    }
    return Some(string[start_index + 1..end_index].to_string());
}

pub fn get_first_token(string: &String, end_char: char) -> Option<String> {
    return match string.split_once(end_char) {
        Some(s) => Some(s.0.to_string()),
        None => None,
    };
}

pub fn get_last_token(string: &String, end_char: char) -> Option<String> {
    return match string.rsplit_once(end_char) {
        Some(s) => Some(s.1.to_string()),
        None => None,
    };
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_my_string() {
        assert_eq!(
            cut(&String::from("(test)"), '(', ')'),
            Some(String::from("test"))
        );
        assert_eq!(
            get_first_token(&String::from("test,"), ',',),
            Some(String::from("test"))
        );
        assert_eq!(
            get_first_token(&String::from("test("), '(',),
            Some(String::from("test"))
        );
        assert_eq!(
            get_last_token(&String::from("test=Test"), '=',),
            Some(String::from("Test"))
        );
    }
}
