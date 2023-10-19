void arch__exit()
{
    asm("cli");
    for (;;)
    {
        asm("hlt");
    }
}