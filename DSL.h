#define POP( stack_ptr )         \
    StackPop(stack_ptr);

#define PUSH( stack_ptr, val )   \
    SyackPush(stack_ptr, val);
