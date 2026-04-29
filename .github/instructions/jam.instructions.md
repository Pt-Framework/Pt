---
applyTo: "**/*.jam,**/Jamfile,**/Jamconfigure"
description: "Writing and modifying Jam scripts"
---

# Jam Scripting Instructions

When writing or modifying `.jam` files for the Pt build system, keep the following language-specific behaviors in mind:

## The `return` statement does not break control flow
In Jam, the `return` statement sets the return value of a rule but **does not** immediately exit the rule's execution. Execution will continue with the next statement after the `return` statement. 

To conditionally avoid executing the rest of a rule, you must wrap the remaining code in `if`/`else` blocks instead of using an "early return" pattern.

**Incorrect (Early Return Attempt):**
```jam
rule MyRule
{
    if $(ALREADY_DONE)
    {
        return ; # Does NOT exit the rule!
    }

    # This code will still run even if ALREADY_DONE is true
    DoSomething ;
}
```

**Correct:**
```jam
rule MyRule
{
    if ! $(ALREADY_DONE)
    {
        DoSomething ;
        ALREADY_DONE = 1 ;
    }
}
```