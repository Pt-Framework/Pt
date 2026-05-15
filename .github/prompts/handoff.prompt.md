---
name: "handoff"
description: "Summarizes the relevant working context for a new Copilot chat"
argument-hint: "Optional: topic, bug, or focus"
agent: "agent"
---
Create a handoff for a new Copilot chat.

Summarize only the relevant working context and leave out irrelevant intermediate steps.

Provide exactly these sections:

## Goal
## Current State
## Affected Files and Symbols
## Known Bugs, Risks, and Uncertainties
## Tested Hypotheses and Validation
## Next Concrete Step

Requirements:
- Keep it short and technically precise
- Do not repeat information
- Include only information that is relevant for continuing the work in the new chat
- If build, test, or reproduction steps are known, include them in the appropriate section
- If there are open decisions, list the alternatives and give a short reason for each
