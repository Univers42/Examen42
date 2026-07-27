# include "suite/print.hpp"

int main() {
    print("hello", 42, 3, 14);
    success("build passed");
    error("something broke: ", 7);
    warn("low disk space", "urgent", "my disk is in fire");
    info("starting up...");
}
