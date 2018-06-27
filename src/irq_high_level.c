

extern void terminal_writestring(const char* data);

void irq_common_high_level(int irq_code)
{
	terminal_writestring("Interrupt detected");
}

void interrupt_handler(int error_code)
{
	terminal_writestring("Interrupt");
}
