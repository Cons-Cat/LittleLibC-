#include <allocators>
#include <array>
#include <math>

int4 global_int_1 = 0;
int4 global_int_2 = 0;

struct TestType {
    TestType() {
        global_int_1++;
    }
    ~TestType() {
        global_int_2++;
    }
};

void meow() {
    // Initialize an allocator.
    cat::PageAllocator allocator;
    // Allocate a page.
    Optional maybe_memory = allocator.malloc<int4>(1_ki);
    if (!maybe_memory.has_value()) {
        cat::print_line("Failed to allocate memory!").discard_result();
        cat::exit(1);
    }
    auto memory = maybe_memory.value();

    // Write to the page.
    allocator.get(memory) = 10;
    Result(allocator.get(memory) == 10).or_panic();
    // Free the page.
    allocator.free(memory).or_panic();

    // Allocation with small-size optimization.
    int stack_variable;
    auto small_memory = allocator.malloca<int4>().value();
    allocator.get(small_memory) = 2;
    // Both values should be on stack, so these addresses are close
    // together.
    Result(cat::abs(intptr{&stack_variable} -
                    intptr{&allocator.get(small_memory)}) < 512)
        .or_panic();
    allocator.freea(small_memory).discard_result();

    small_memory = allocator.malloca<int4>(1_ki).value();
    // `small_memory` should be in a page, so these addresses are far
    // apart.
    Result(cat::abs(intptr{&stack_variable} -
                    intptr{&allocator.get(small_memory)}) > 512)
        .or_panic();
    allocator.freea(small_memory).discard_result();

    // Test constructor being called.
    Optional testtype = allocator.malloc<TestType>();
    allocator.free(testtype.value()).discard_result();

    // That constructor increments `global_int_1`.
    Result(global_int_1 == 1).or_panic();
    // That destructor increments `global_int_2`.
    Result(global_int_2 == 1).or_panic();

    // Optional smalltesttype = allocator.malloca<TestType>();
    // allocator.freea(smalltesttype.value()).discard_result();

    cat::exit();
};
