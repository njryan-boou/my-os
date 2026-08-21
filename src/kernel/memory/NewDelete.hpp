#pragma once

namespace kernel::memory {

class KernelHeap;

void initialize_new_delete(KernelHeap& heap);

}