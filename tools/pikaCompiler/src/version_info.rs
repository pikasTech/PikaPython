use crate::my_string;
use std::collections::BTreeMap;
use std::fs::File;
use std::io::prelude::*;

pub struct VersionInfo {
    pub source_path: String,
    pub package_list: BTreeMap<String, String>,
}

impl VersionInfo {
    pub fn new() -> VersionInfo {
        let version_info = VersionInfo {
            /* default source_path */
            source_path: String::from(""),
            package_list: BTreeMap::new(),
        };
        return version_info;
    }

    pub fn analyze_file(mut self, source_path: String) -> VersionInfo {
        self.source_path = source_path;
        let file = File::open(&self.source_path);
        let mut file = match file {
            Ok(file) => file,
            Err(_) => {
                println!("(PikaScript) requestment.txt no found.");
                return self;
            }
        };
        return self;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_analyze() {
        let mut version_info = VersionInfo::new();
        version_info = VersionInfo::analyze_file(version_info, String::from("test_source_path"));
        assert_eq!(version_info.source_path, "test_source_path");
    }
}
