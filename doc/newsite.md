# New website concept

Stand der Diskussion: zwei Wurzeln (Home / Docs), vier Header-Kacheln,
dünne Get-Platinum-Seite, Docs-Root aus Portalen und Modulkacheln,
auf API-Seiten eine schmale Leiste plus Doxygen-Navpath.

Statische Site ohne JS-Framework, CDN oder Theme-Repo. Doxygen darf auf
eine aktuelle Version (1.14+, Ziel 1.18) wechseln. `HTML_EXTRA_FILES`
nur für Assets, nicht für Inhalt.

Farbe Akzent: **RGB(150, 0, 50)** (`#960032`). Kein Crimson.

## Ziele

- Layout in der Art Boost: Marketing-Home und Docs-Root, nicht ein Mega-Menü.
- Von jeder Unterseite direkt zu allen Oberseiten (Header), Sprünge zwischen
  Ästen über das TOC.
- Hero-Buttons zeigen nicht auf Ziele, die schon im Header liegen.
- Eine Chrome-Quelle (Header/Footer/CSS), ein Linkraum, kein `htdocs/`-Schnitt.

## Navigation

### Oberseiten vs. Baum

| Ziel | Wo |
|---|---|
| Home | Logo-Kachel |
| Docs-Root | Docs-Kachel |
| GitHub (Repo) | GH-Kachel |
| Download | Hero auf der Home **und** TOC (keine Header-Kachel) |
| anderer Ast (Jam, Core, Class Index, …) | TOC |

Aktiver Zustand nur an den Wurzeln:

```
Home / Download:   [Pt*] [≡] [Docs ] [GH]
alles unter Docs:  [Pt ] [≡] [Docs*] [GH]
```

### Header (jede Seite)

Vier gleich große Kacheln, linksbündig. Weitere Kacheln hängen rechts an,
gleiche Größe. Keine Textmenü-Leiste, keine zweite globale Leiste.

```
+------+  +------+  +------+  +------+
|  Pt  |  |  ≡   |  | Docs |  |  GH  |
+------+  +------+  +------+  +------+
  Home      TOC      Docs-     GitHub
                     Root
```

- Logo führt immer zur Home.
- Hamburger öffnet das TOC (Site-Map der Äste, nicht Seiten-Outline).
- Docs führt immer zur Docs-Root.
- GH führt zum Repository, nicht zu Releases.

Kein Fork-me-Ribbon.

### TOC (Hamburger)

Feste Site-Map, auf jeder Seite gleich. Kern sind die Äste. Oberseiten
dürfen kurz vorkommen, Pflicht ist Download (keine Header-Kachel).

Ohne JS: Checkbox-Hack, Panel von links, Seite gedimmt.

```
+------+  +======+  +------+  +------+
|  Pt  |  |  ≡   |  | Docs |  |  GH  |
+------+  +======+  +------+  +------+
              |
              v
         +---------------------------+
         |  Home                     |   optional (Logo reicht)
         |  Documentation            |   optional (Docs-Kachel reicht)
         |  Download                 |   ja
         |  ----------------         |
         |  Getting Started          |
         |  Reference                |
         |  Jam                      |
         |  ----------------         |
         |  Core                     |
         |  System                   |
         |  Net                      |
         |  HTTP   …                 |
         |  ----------------         |
         |  Class Index              |
         |  Namespaces               |
         |  Modules                  |
         |  License                  |
         +---------------------------+
```

Nicht Doxygens Treeview. Lokales Seiten-Outline (Member, Abschnitte)
bleibt im Inhalt der API-Seite, nicht im Hamburger.

## Home (`@mainpage`)

Hero-Spruch: **The Power in your Hands**.

Buttons nur auf Dinge, die nicht im Header sind: Download-Seite und
Getting Started (erste Portal-Gruppe).

```
+------+  +------+  +------+  +------+
|  Pt* |  |  ≡   |  | Docs |  |  GH  |
+------+  +------+  +------+  +------+

  THE POWER IN YOUR HANDS

  [ Download ]    [ Getting Started ]

  Windows · Linux · macOS · QNX · Unix · Wasm
  LGPL 2.1 with linking exception


  What you ship
  +----------------+ +----------------+ +----------------+ +----------------+
  | Native UI      | | Networked      | | Systems        | | Graphics       |
  +----------------+ +----------------+ +----------------+ +----------------+

  How it is built
  +-------------------+ +-------------------+ +-------------------+
  | Standard C++      | | Async by default  | | Modular           |
  +-------------------+ +-------------------+ +-------------------+
```

Keine volle Modulliste auf der Home (die steht auf der Docs-Root).
Wenn der untere Teil zu lang wird: „What you ship“ streichen, die drei
„How it is built“-Karten behalten.

## Download (Option B)

Dünne Seite **Get Platinum**. Nicht im Header. Hero und TOC landen hier.
Kein Bounce nur auf GitHub Releases.

Clone ist der Alltag. Darunter das aktuelle **Framework-Release** (`v*`).
Prebuilts nicht als Download anbieten — die holt Jam
(`--fetch-prebuilt`). Auf der GitHub-Release-Liste stehen `v*` und
`prebuilt-*` nebeneinander; die Seite muss das trennen.

```
+------+  +------+  +------+  +------+
|  Pt  |  |  ≡   |  | Docs |  |  GH  |
+------+  +------+  +------+  +------+

  Get Platinum

  git clone https://github.com/Pt-Framework/Pt.git


  Source release
  [ v2.0.0-pre3 ]          all releases →

  Framework tags start with v.
  prebuilt-* archives are dependencies, not the framework.


  Next:  Installing from Source
```

- Primärbutton: aktueller Framework-Tag (`#960032`).
- „All releases“: Textlink nach GitHub.
- Kein Fork-Button, kein Source-zip von `main` als Default.
- Versionsnummer im Button ist eine Zeile in der Seite, die beim Taggen
  mitgezogen wird.

`https://github.com/Pt-Framework/Pt/releases/latest` erst verwenden, wenn
ein Nicht-Prerelease existiert. Bis dahin den konkreten Tag verlinken
(aktuell `v2.0.0-pre3`).

Später bei einem echten Release nur Button-Text und Ziel ändern
(`v2.0.0` → `/releases/latest`). Seite, Header und Hero bleiben.

## Docs-Root

Einstieg in die Dokumentation. Drei Portal-Kacheln, darunter die Module.

Lautstärke Portal ≈ Modul. Trennung **A+B**:

- **A:** Portale eigene Reihe (drei Spalten), Module Grid darunter.
- **B:** Akzentbalken `#960032` nur an Portalen.
- Kicker **Libraries** nur über den Modulen, keiner über den Portalen.

Module behalten die Unterlinks als Überblick.

Reference, nicht Overview: Class Index / Namespaces / Modules
(der alte Docs-Header, als Kachel).

```
+------+  +------+  +------+  +------+
|  Pt  |  |  ≡   |  |Docs* |  |  GH  |
+------+  +------+  +------+  +------+

  Documentation

  +----------------------+ +----------------------+ +----------------------+
  |# Getting Started     | |# Reference           | |# Jam                 |
  |#                     | |#                     | |#                     |
  |  Install from source | |  Class Index         | |  Introduction        |
  |  Platforms           | |  Namespaces          | |  Usage               |
  |  License             | |  Modules             | |  Prebuilt deps       |
  +----------------------+ +----------------------+ +----------------------+
     Balken RGB(150,0,50)


  Libraries

  +----------------------+ +----------------------+ +----------------------+
  | Core                 | | System               | | Net                  |
  | Arguments            | | File system          | | Endpoints            |
  | Settings             | | Concurrency          | | TCP                  |
  | Events               | | Event loop           | | UDP                  |
  | …                    | | …                    | |                      |
  +----------------------+ +----------------------+ +----------------------+
  | HTTP                 | | SSL                  | | Forms                |
  | …                    | | …                    | | …                    |
  +----------------------+ +----------------------+ +----------------------+
```

`#` = Akzentbalken. Getting Started, Reference, Jam sind Gruppen auf
dieser Seite (Links in der Kachel), keine Header-Punkte.

## API-Seiten

Header unverändert. Darunter **eine** schmale Zeile, nur auf
Klasse / Namespace / Member / File / Group:

- links Inventar (Querschnitt der Referenz)
- rechts Doxygen-Navpath (Standort in der Gruppe)

`DISABLE_INDEX = YES` — keine Doxygen-Tabs. Navpath an. API-Links sind
eigenes HTML im Header, nicht Doxygens Index.

```
+------+  +------+  +------+  +------+
|  Pt  |  |  ≡   |  |Docs* |  |  GH  |
+------+  +------+  +------+  +------+

 Class Index    Namespaces    Modules          Pt › Gfx › PaintSurface
----------------------------------------------------------------
  PaintSurface Class Reference
  Graphics and Imaging  »  Drawing Graphics and Text
```

Die Zeile ist auf Home, Download, Docs-Root, Installing, Jam **aus**
(gleiche Header-Datei, CSS anhand des Seiten-Markups).

Navpath: klein, `#a8a8a8`, Trennzeichen `›`, letzter Brocken etwas heller.
Kein Balken `#960032` am Pfad (der bleibt den Portalen).

`Main` aus der alten Subnav entfällt — das ist die Docs-Kachel.

### Wer wohin

```
Pt::PaintSurface
    Docs-Kachel     →  Docs-Root
    Class Index     →  A–Z (classes.html)
    Gfx im Navpath  →  Gfx-Gruppe / Pt-Gfx-Page
    TOC             →  Jam, Installing, anderes Modul
    Pt-Kachel       →  Home
```

Die **Seiten** (`classes.html`, `classPt_1_1String.html`, …) generiert
Doxygen weiter. Der Weg dorthin ist die API-Zeile plus TOC, nicht
Doxygens Default-Tabs und nicht der Treeview.

## Was bewusst fehlt

- Features-Bereich, About-Punkt, Community-Seite
- Fork-Ribbon
- Docs-Button im Hero
- Download als fünfte Header-Kachel
- Doxygen-Treeview als Hauptnavigation
- Suche (später, unten im TOC)
- Artwork (Logo-Kachel, Hamburger, Hero) — später

Footer: Copyright, License, GitHub. Mehr nicht.

Contributing / Security: Links in Getting Started, nicht im Header.

## Doxygen

Eine generierte Site, ein Output-Ordner (kein Marketing-Baum neben
`htdocs/`).

| Seite | Mechanismus |
|---|---|
| Home | `@mainpage` / `USE_MDFILE_AS_MAINPAGE`, Hero per `\htmlonly` |
| Download | `\page` |
| Docs-Root | `\page documentation` |
| Installing, Platforms, License, Jam, Module | bestehende `\page`s |
| Header / Footer | neu aus `doxygen -w html`, vier Kacheln + API-Zeile + `$navpath` |
| CSS | Default behalten, Overlay (`HTML_EXTRA_STYLESHEET`), nicht `HTML_STYLESHEET` ersetzen |
| Extra files | Logo, Favicon, Bilder |
| Index-Tabs | aus (`DISABLE_INDEX = YES`) |
| Treeview | aus (TOC ist eigenes HTML) |

Alte URLs (`/htdocs/…`, `/features-*.html`) brauchen Redirects oder
brechen.

`features-license.html` / `features-platforms.html` gehen in
`license.md` / `platforms.md`. `docs.html` wird die Docs-Root.
`index.html` wird die Mainpage. `downloads.html` wird die Get-Platinum-Seite.

## Abbildung heutiger Dateien

| Heute | Neu |
|---|---|
| `doc/website/index.html` | Mainpage |
| `doc/website/docs.html` | Docs-Root |
| `doc/website/downloads.html` | Get Platinum (`\page`) |
| `doc/website/features-modules.html` | entfällt (Home-Karten + Modulgrid) |
| `doc/website/features-design.html` | Abschnitte auf der Home |
| `doc/website/features-license.html` | `license.md` |
| `doc/website/features-platforms.html` | `platforms.md` |
| `doc/pt_header.html` | neu: Kacheln + API-Zeile + Navpath |
| `doc/doxygen.css` + `site.css` | Overlay auf Doxygen-Default, Akzent `#960032` |
