extern crate cc;

fn main() {
    cc::Build::new().file("libnp/libnp.c").compile("libnp");
}
