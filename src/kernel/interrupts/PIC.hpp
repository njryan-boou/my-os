#pragma once

namespace kernel::interrupts {

void initialize_pic();
void send_eoi();

}