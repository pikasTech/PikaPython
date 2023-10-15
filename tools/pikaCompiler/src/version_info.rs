use std::collections::BTreeMap;
use std::fs::File;
use std::io::prelude::*;

pub struct VersionInfo {
    pub source_path: String,
    pub package_list: BTreeMap<String, String>,
    pub plot: bool,
}

impl VersionInfo {
    pub fn new() -> VersionInfo {
        let version_info = VersionInfo {
            /* default source_path */
            source_path: String::from(""),
            package_list: BTreeMap::new(),
            plot: true,
        };
        return version_info;
    }

    fn analyse_line(mut self, line: String) -> VersionInfo {
        /* delete '\r' */
        let line = line.replace("\r", "");
        /* skip void line */
        if line.trim().is_empty() {
            return self;
        }
        /* print the package info */
        if self.plot {
            // print line with \r\n
            print!("  {}{}", line, "\r\n");
        }

        // Check if line contains '='
        if let Some(pos) = line.find('=') {
            let package_name = line[..pos].trim().to_string();
            let package_version = line[pos + 1..].trim().to_string();

            // skip pikascript-core
            if package_name == "pikascript-core" {
                return self;
            }

            self.package_list
                .entry(package_name)
                .or_insert(package_version);
        } else {
            // For formats like "json" without a version.
            let package_name = line.trim();
            self.package_list
                .entry(package_name.to_string())
                .or_insert("".to_string()); // Assuming you want to insert an empty string for versions.
        }

        return self;
    }

    pub fn analyse_file(mut self, source_path: String) -> VersionInfo {
        self.source_path = source_path;
        let file = File::open(&self.source_path);
        let mut file = match file {
            Ok(file) => file,
            Err(_) => {
                println!("    'requestment.txt' no found.");
                return self;
            }
        };
        /* solve lines in file */
        let mut file_str = String::new();
        file.read_to_string(&mut file_str).unwrap();
        let lines: Vec<&str> = file_str.split('\n').collect();
        /* analyse each line of pikascript-api.pyi */
        for line in lines.iter() {
            self = VersionInfo::analyse_line(self, line.to_string());
        }
        return self;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_analyse() {
        let mut version_info = VersionInfo::new();
        version_info = VersionInfo::analyse_file(version_info, String::from("test_source_path"));
        assert_eq!(version_info.source_path, "test_source_path");
    }
}
