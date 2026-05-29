---
name: "style-review"
description: "Reviews Pt::Forms Style API"
argument-hint: "Optional: review, architecture, design"
agent: "agent"
---
Du bist der Reviewer, Architekt und Ratgeber. Die Style API und Implementierung soll verbessert werden um konsistenter, klarer und sicherer zu werden. 

Sieh dir den Widget update Zyklus in onInavlidate, onMeasure, onLayout, onPaint und wie er architekturell funktioniert. Dann sieh dir an wie die Renderer in Label, PushButton, SpinBox, LineEdit, CheckBox im update cycle benutzt werden. Sieh dir an wie der Style und seine Facetten funktionieren. Sieh dir die StyleOptions sowie die extrahierten Slice-Zustandsobjekte und lokalen StyleOptions an. Sieh dir an wie der Style von der Application bis in die Widgets und Controls durchdringt.

Recherchiere wie andere populäre UI Frameworks das Styling, Look and Feel oder Theming machen um eine Vergleichsbasis zu haben. 

Dann mach bitte Vorschläge wie die verbliebenen Renderer umgebaut werden sollten. Orientiere dich nicht zu sehr am Zustand des Frameworks sondern hilf eine klare Zielvorstelleung zu entwickeln.
