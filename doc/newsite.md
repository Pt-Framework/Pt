# New website concept

Stand: die generierte Doxygen-Site (1.18). Zwei Wurzeln (Home / Docs),
Header als Krume, dünne Get-Platinum-Seite, Docs-Root aus Portalen und
Modul-Kacheln gleicher Form, auf API-Seiten eine Kontextzeile (Inventar und
Navpath) plus Doxygen-Titel als Überschrift, keine dritte Leiste.

Statische Site ohne JS-Framework, CDN oder Theme-Repo.
`HTML_EXTRA_FILES` nur für Assets, nicht für Inhalt.

Farbe Akzent: **RGB(150, 0, 50)** (`#960032`). Kein Crimson.
Hintergrund **RGB(24, 24, 24)** (`#181818`). Kacheln **RGB(31, 31, 31)**
(`#1f1f1f`). Buttons **RGB(42, 42, 42)** (`#2a2a2a`). Primärbutton bleibt Akzent.
Titel und helle Labels **RGB(238, 238, 238)** (`#eeeeee`). Inhaltstext und
Listen in Kacheln dunkler (`#a8a8a8`). Links und besuchte Links dieselbe
Farbe (`#a8a8a8`), kein Doxygen-Blau. Ghost-Button-Hover: Fläche und
Rahmen leicht aufgehellt, ohne Akzent.

Zweite Farbe ist Azur, Hue **208°**, eine Leiter. Rot bleibt der satte
Strukturakzent. Namen auf der 400er-Stufe, Kästen auf 900/950:

| Stufe | HSL | Hex | Rolle |
|---|---|---|---|
| 950 | 208° 50% 11% | `#0E1D2A` | Clone-Box und Codefragmente |
| 700 | 208° 32% 28% | `#30495E` | Rand nur an Clone/Code |
| 400 | 208° 48% 54% | `#518DC2` | Class-Index, Member- und Klassennamen |
| 300 | 208° 16% 70% | `#A8B7C4` | Text in der Codebox |

Parameter in Signaturen **RGB(163, 146, 116)** (`#A39274`).

Quellen: `doc/pages/*.md`, `doc/pt_header.html`, `doc/pt_footer.html`,
`doc/site.css`. Output: `doc/website/` (flach, `HTML_OUTPUT = .`).

## Ziele

- Layout in der Art Boost: Marketing-Home und Docs-Root, kurzes Menü.
- Von jeder Unterseite zu den Oberseiten (Header-Krume). Download, Guides,
  Jam und API-Index über das Hamburger-Menü. Framework-Module über Home-
  Karten und Docs-Root.
- Hero-Buttons: Download, Getting Started, Documentation, GitHub.
- Eine Chrome-Quelle (Header/Footer/CSS), ein Linkraum, kein `htdocs/`-Schnitt.

## Navigation

### Oberseiten vs. Baum

| Ziel | Wo |
|---|---|
| Home | Wortmarke `Pt` |
| Docs-Root | Krume `Documentation` |
| Download | Krume `Get Platinum` (nur auf der Download-Seite), Hero und Menü |
| GitHub (Repo) | Hero auf Home und Footer |
| Getting Started, License, Jam, API-Index | Menü; Getting Started auch im Hero |
| Framework-Module (Core, Gfx, …) | Home-Karten und Docs-Root |

Header ohne Akzent. Aktiver Brocken dieselbe Leistenfarbe, kein gefülltes Quadrat:

```
Home:          Pt*                                             ≡
Download:      Pt  ›  Get Platinum*                            ≡
Docs-Root:     Pt  ›  Documentation*                           ≡
Guides / API:  Pt  ›  Documentation*                           ≡
```

### Header (jede Seite)

Eine Textleiste. Links die Krume, rechts der Hamburger. Keine Kacheln,
keine Trennlinie unter der Leiste, keine zweite globale Leiste, kein Docs-Link
als eigener Punkt rechts.

```
Home:          Pt                                              ≡
Download:      Pt  ›  Get Platinum                             ≡
übrige Seiten: Pt  ›  Documentation                            ≡
```

- Logo-Text `Pt` führt immer zur Home (`index.html`). Artwork später: derselbe
  Link, Klasse `.pt-nav-home`, Wortmarke wird Icon. Slot 32×32px, zentriert
  in der 48px-Leiste — gleicher Kasten wie der Hamburger.
- `Documentation` führt zur Docs-Root (`docs.html`). Auf der Docs-Root der
  aktuelle Brocken (Leistenfarbe, kein Link).
- `Get Platinum` nur auf der Download-Seite, aktueller Brocken.
- Hamburger öffnet das Menü rechts unter der Leiste. Zeichen `≡`, 30px im
  32×32-Slot, gleiche Hit-Area wie `.pt-nav-home`. Offen bleibt die
  Leistenfarbe (kein Akzent).
- Krume (`Pt`, `›`, `Documentation` / `Get Platinum`) sitzt in derselben
  32px-Höhe, vertikal zentriert.
- GitHub steht im Hero auf Home und im Footer, nicht in der Leiste.

Seitenmarker steuern die Krume (CSS `:has()`, kein JavaScript):
`.pt-page-home`, `.pt-page-download`, `.pt-page-docs`, sonst Docs-Krume.

Kein Fork-me-Ribbon. Kein Doxygen-Tab-Index (`DISABLE_INDEX = YES`).

### Menü (Hamburger)

Auf jeder Seite gleich. Checkbox/CSS, Panel rechts unter der Leiste am
Hamburger, Seite unter der Leiste gedimmt. Kein JavaScript. Keine
Framework-Module in der Liste.

```
Start
  Download
  Getting Started
  License

References
  Jam Build Tool
  Class Index
  Namespaces
  Modules
```

| Eintrag | Ziel |
|---|---|
| Download | `downloads.html` |
| Getting Started | `docs.html#getting-started` |
| License | `license.html` |
| Jam Build Tool | `docs.html#jam` |
| Class Index | `classes.html` |
| Namespaces | `namespaces.html` |
| Modules | `topics.html` (Doxygen-Gruppen) |

Home und Documentation stehen nicht im Panel (Krume reicht).
Contributing und Security nur im Getting-Started-Portal (GitHub).

Überschriften `START` / `REFERENCES` sind Labels (`div`, keine Links):
`#eeeeee`, 11px, Gewicht 700, uppercase, Laufweite 0.12em. Einträge 14px,
Gewicht 400, `#a8a8a8`, Padding 5px 12px, dieselbe linke Kante wie die
Labels. Fläche `#1f1f1f`, Rahmen `#3a3a3a`, Hover-Text `#eeeeee` ohne
Flächen-Highlight. Panel `max-content`, `right: 16px`.

Nicht Doxygens Treeview (`GENERATE_TREEVIEW = NO`). Lokales Seiten-Outline
(Member, Abschnitte) bleibt im Inhalt der API-Seite, nicht im Hamburger.

## Home (`@mainpage`)

Datei `doc/pages/mainpage.md` → `index.html`. Hero-Spruch:
**The Power in your Hands**. Auf schmalen Viewports bricht der Titel um
(kein Abschneiden). Doxygen-Titel aus auf Home, Download und Docs-Root (`.header` auf
`.pt-page-home` / `.pt-page-download` / `.pt-page-docs`). Eigene Titel
in der Seite. Auf übrigen `\page`s und API: `.title` ohne Doxygen-Balken,
`\section` visuell unter dem Seitentitel.

Hero-Buttons: Download-Seite, Getting Started (Anker der Portal-Gruppe auf
der Docs-Root), Documentation (Docs-Root) und GitHub (Repository).

```
Pt*                                                    ≡

  THE POWER IN YOUR HANDS
  Portable · Modular · Asynchronous · Standard C++ · Flexible · Open and proprietary

  Platinum (Pt) is a comprehensive C++ framework, which allows developers
  to write high-performance applications for many platforms with only one
  codebase. It provides a large amount of features and is still very easy
  to use. It integrates well into existing toolkits and frameworks.

  [ Getting Started ]    [ Documentation ]    [ GitHub ]              [ Download ]


  Modules                (eine Kachel pro Modul, Rahmen, eine Zeile)
  Core | System | Net | HTTP | SSL | Gfx | Forms | XmlRpc |
  Unit | Database | Lua | MCP | Reflex | Cosmo

  Design                 (vier Textblöcke, Akzentlinie links)
  Standard C++ | Asynchronous stack | Modular and non-intrusive | One codebase
```

Home ist das Argument, Docs-Root das Inventar.

- **Eigenschaften:** eine Zeile unter dem Titel, vor dem Lead, **keine Links**.
  Download im Hero rechts, die drei Ghost-Buttons links.
  Plattformen und Lizenz liegen auf ihren Seiten, nicht in dieser Zeile.
- **Modules:** Karten mit Rahmen, ohne Akzentbalken. Titel plus eine Zeile.
  Klick auf die Modulseite. Keine Kapitel-Unterlinks (die stehen auf der
  Docs-Root). Reflex ohne Zielseite ist eine Karte ohne Link.
- **Design:** keine Kacheln, Textblöcke mit linker Linie `#960032`. Nimmt
  die alte Design Philosophy und die frühere „Why Platinum?“-Sektion auf.
  Keine dritte Bullet-Liste.

## Download

Datei `doc/pages/downloads.md`, `\page downloads` → `downloads.html`.
Dünne Seite **Get Platinum**. Nicht dauerhaft in der Leiste. Hero und Menü landen hier.
Kein Bounce nur auf GitHub Releases.

Clone ist der Alltag. Darunter das aktuelle **Framework-Release** (`v*`).
Prebuilts nicht als Download anbieten — die holt Jam (`--fetch-prebuilt`).

```
Pt  ›  Get Platinum*                                       ≡

  Get Platinum

  git clone https://github.com/Pt-Framework/Pt.git

  Source release
  [ v2.0.0-pre3 ]          all releases →

  Framework tags start with v.
  prebuilt-* archives are dependencies, not the framework.

  A snapshot of main is also available as a source zip.

  Next:  Installing from Source
```

- Primärbutton: aktueller Framework-Tag (`#960032`).
- „All releases“: Textlink nach GitHub.
- Kein Fork-Button. Source-zip von `main` nur als Textlink, nicht als Default.
- Versionsnummer im Button ist eine Zeile in der Seite, die beim Taggen
  mitgezogen wird.

`https://github.com/Pt-Framework/Pt/releases/latest` erst verwenden, wenn
ein Nicht-Prerelease existiert. Bis dahin den konkreten Tag verlinken
(aktuell `v2.0.0-pre3`).

Später bei einem echten Release nur Button-Text und Ziel ändern
(`v2.0.0` → `/releases/latest`). Seite, Header und Hero bleiben.

## Docs-Root

Datei `doc/pages/docs.md`, `\page docs` → `docs.html`.
Die Page-ID ist **`docs`**, nicht `documentation` (sonst wäre die Datei
`documentation.html` und bestehende `/docs.html`-Links wären tot).

Einstieg in die Dokumentation. Drei Portal-Kacheln, darunter die Module.

- Portale eigene Reihe (drei Spalten), Module Grid darunter, gleiche Kachelform.
- Akzentbalken `#960032` an Portalen **und** Modul-Kacheln.
- Kicker **Modules** nur über den Modul-Kacheln.
- Pro Modul: nur der Name als Überschrift und Links zu den Kapiteln.
  Keine Beschreibungszeile.

```
Pt  ›  Documentation*                                      ≡

  Documentation

  +----------------------+ +----------------------+ +----------------------+
  |# Getting Started     | |# Reference           | |# Jam Build Tool      |
  |  Installing from     | |  Class Index         | |  Introduction        |
  |    Source            | |  Namespaces          | |  Getting Jam         |
  |  Supported Platforms | |  Modules             | |  Basic Usage         |
  |  License             | |                      | |  Using the Jambase   |
  |  README              | |                      | |  Prebuilt Dependencies|
  |  Contributing        | |                      | |                      |
  |  Security            | |                      | |                      |
  +----------------------+ +----------------------+ +----------------------+


  Modules

  +----------------------+ +----------------------+ +----------------------+
  |# Core                | |# System              | |# Net                 |
  |  Command Line …      | |  File System Access  | |  Endpoints           |
  |  Application Settings| |  Concurrency         | |  TCP Sockets         |
  |  …                   | |  …                   | |  UDP Sockets         |
  +----------------------+ +----------------------+ +----------------------+
  (gleiche Form für HTTP, SSL, Gfx, Forms, XmlRpc, Unit, Database,
   Lua, MCP, Reflex, Cosmo)
```

`#` = Akzentbalken. Getting Started, Reference, Jam Build Tool sind Gruppen auf
dieser Seite (Links in der Kachel), keine Header-Punkte.
Portal- und Modul-Kacheln: Überschrift `#eeeeee`. Links ohne Unterstreichung,
auch beim Hover.

Home-Modul-Kacheln: Rahmen, eine Zeile, kein Balken. Docs-Modul-Kacheln:
Portalform, Kapitel-Links, kein Fließtext.

README, Contributing und Security zeigen auf GitHub. Die übrigen Ziele
sind generierte `\page`s.

## API-Seiten

Zwei Ebenen, keine Trennlinien. Site-Leiste mit Docs-Krume. Darunter **eine**
Kontextzeile, nur auf Klasse / Namespace / Member / File / Group / Index:

- links Inventar: Class Index · Namespaces · Modules
- rechts Doxygen-Navpath (Standort), in derselben Flex-Zeile (`#top`)

`DISABLE_INDEX = YES` — keine Doxygen-Tabs. Navpath an. API-Links sind
eigenes HTML im Header, nicht Doxygens Index. Doxygen-`.header` ist der
Seitentitel, keine dritte Leiste. Titel und Modul-Elternlink zentriert.
Darunter linksbündig Include, dann Brief. `.summary` (Public Member
Functions | …) ist aus; die Abschnitte stehen im Member-Block. Member-
Namen in Azur der Class-Index-Namen (`#518DC2`). Briefs unter der
Signatur, klassisches zweispaltiges Raster, keine Member-Karten.
Member-Details: Fläche `#1f1f1f` ohne Rahmen, 8px Abstand zwischen den
Blöcken. Azur nur als Namensfarbe.

```
Pt  ›  Documentation*                                      ≡
 Class Index · Namespaces · Modules     Pt › Gfx › PaintSurface

                 PaintSurface
                 Gfx Module

  #include <Pt/Gfx/PaintSurface.h>
  Brief...

  Public Member Functions
    template...
                         name (args)
                         Brief.
```

Die Kontextzeile ist auf Home, Download und Docs-Root **aus** (Seitenmarker
`.pt-page-home` / `.pt-page-download` / `.pt-page-docs`). Auf übrigen
`\page`s ohne Klassen-/Group-Markup bleibt sie ebenfalls aus.

Navpath: gleiche Schriftgröße wie das Inventar, `#a8a8a8`, Trennzeichen `›` in `1em` der Zeile, letzter Brocken etwas heller. Inventar mit Mittelpunkten: Class Index · Namespaces · Modules.
Keine Doxygen-Breadcrumb-Grafiken. Kein Balken `#960032` am Pfad (der bleibt
Portalen und Docs-Modul-Kacheln).

`Main` aus der alten Subnav entfällt — das ist die Krume Documentation.

### Wer wohin

```
Pt::PaintSurface
    Documentation   →  Docs-Root (docs.html)
    Class Index     →  A–Z (classes.html)
    Gfx im Navpath  →  Gfx-Gruppe / Pt-Gfx-Page
    Menü            →  Download, Getting Started, Jam, API-Index
    Pt              →  Home
```

Die **Seiten** (`classes.html`, `classPt_1_1String.html`, …) generiert
Doxygen weiter. Der Weg dorthin ist die API-Zeile plus Menü, nicht
Doxygens Default-Tabs und nicht der Treeview.

## Chrome und Layout

- Overlay `doc/site.css` (`HTML_EXTRA_STYLESHEET`), Doxygen-Default bleibt.
- Sticky Site-Leiste, darunter auf API-Seiten eine sticky Kontextzeile.
  Hover nur CSS. Keine horizontalen Trennlinien im Chrome.
- Footer: Copyright · License · GitHub.
- Favicon über `HTML_EXTRA_FILES`. Logo-Artwork später.
- Responsive ab 800px: Portale, Design und dreispaltige Grids
  untereinander; Hero-Titel kleiner, mit Umbruch.

Kartenrollen:

| Rolle | Ort | Form |
|---|---|---|
| Site-Leiste | überall | Krume links, Hamburger rechts (32×32 wie Pt); keine Akzentfarbe |
| Kontextzeile | API-Seiten | Inventar links, Navpath rechts |
| Eigenschaften | Home unter Hero | Zeile, keine Links, kein Rahmen |
| Module (Home) | Home | Rahmen, Titel + eine Zeile, kein Balken |
| Design | Home | Text, linke Linie `#960032`, kein Rahmen |
| Portale | Docs-Root | Rahmen, Akzentbalken oben, Linklisten |
| Module (Docs) | Docs-Root | gleiche Form wie Portale; Name + Kapitel |

Akzentbalken nur an Docs-Portalen und Docs-Modul-Kacheln. Design nie als Karte.
Header ist eine Leiste, keine Kachel.

## Was bewusst fehlt

- Features-Bereich, About-Punkt, Community-Seite
- Fork-Ribbon
- Download in der Site-Leiste
- Doxygen-Treeview als Hauptnavigation
- Suche (`SEARCHENGINE = NO`; später, unten im Menü)
- Artwork (Logo, Hamburger, Hero) — später
- Server-Redirects für alte URLs (Hosting-Folgeauftrag)

Footer: Copyright, License, GitHub. Mehr nicht.

Contributing / Security: Links im Getting-Started-Portal, nicht im Header
und nicht im Menü.

## Doxygen

Eine generierte Site, ein Output-Ordner (`OUTPUT_DIRECTORY = ./website`,
`HTML_OUTPUT = .`). Kein Marketing-Baum neben `htdocs/`.

| Seite | Mechanismus | Datei |
|---|---|---|
| Home | `USE_MDFILE_AS_MAINPAGE = pages/mainpage.md` | `index.html` |
| Download | `\page downloads` | `downloads.html` |
| Docs-Root | `\page docs` | `docs.html` |
| Installing, Platforms, License, Jam, Module | bestehende `\page`s | wie bisher |
| Header / Footer | `pt_header.html` / `pt_footer.html` | Leiste + Menü + Kontextzeile + `$navpath` |
| CSS | `HTML_EXTRA_STYLESHEET = site.css` | Overlay, nicht `HTML_STYLESHEET` |
| Extra files | Favicon (Logo später) | |
| Index-Tabs | `DISABLE_INDEX = YES` | |
| Treeview | `GENERATE_TREEVIEW = NO` | Menü ist eigenes HTML |
| Suche | `SEARCHENGINE = NO` | |

Alte URLs (`/htdocs/…`, `/features-*.html`) brechen, bis das Hosting
Redirects nachzieht. In-Repo-Links zeigen auf die flachen Pfade.

`features-license.html` / `features-platforms.html` sind in
`license.md` / `platforms.md`. `docs.html` ist die Docs-Root.
`index.html` ist die Mainpage. `downloads.html` ist Get Platinum.

## Abbildung heutiger Dateien

| Quelle | Ziel |
|---|---|
| `doc/pages/mainpage.md` | Home |
| `doc/pages/docs.md` | Docs-Root |
| `doc/pages/downloads.md` | Get Platinum |
| früher `features-modules.html` | Docs-Root-Modul-Kacheln + Home „Modules“ |
| früher `features-design.html` | Home „Design“ |
| `doc/pages/license.md` | License |
| `doc/pages/platforms.md` | Platforms |
| `doc/pt_header.html` | Leiste + Menü + Kontextzeile |
| `doc/pt_footer.html` | Footer |
| `doc/site.css` | Overlay, Akzent `#960032` |
