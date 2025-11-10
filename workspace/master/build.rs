fn main() {
    // This will put data.rs directly into src/
    prost_build::Config::new()
        .out_dir("src/protos") // choose a visible folder inside src/
        .compile_protos(&["proto/data.proto"], &["proto/"])
        .expect("Failed to compile protos!");
    println!("cargo:rerun-if-changed=proto/data.proto");
}
