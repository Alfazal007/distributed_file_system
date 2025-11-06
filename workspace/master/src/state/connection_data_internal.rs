use std::collections::HashMap;

#[derive(Debug)]
pub struct ConnectionData {
    pub connection_id: String,
    pub files_to_chunks: HashMap<String, Vec<String>>,
}
