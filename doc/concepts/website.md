# Website {#website}

The Platinum website is the HTML that Doxygen 1.18 generates from the
public pages, the chrome files, and the overlay stylesheet. It has two
roots: Home is the argument for the framework, and the documentation
inventory is the map of guides and modules. Every other generated page
hangs off one of those roots through a header crumb. The download page
is a thin Get Platinum surface. API class, namespace, member, file,
group, and index pages add one context line under the header, with the
Doxygen title as the heading and no third global bar.

This chapter covers:

- [Purpose](#website-purpose)
- [Color](#website-color)
- [Navigation](#website-navigation)
- [Home](#website-home)
- [Download](#website-download)
- [Documentation inventory](#website-docs)
- [API pages](#website-api)
- [Chrome and layout](#website-chrome)
- [Scope](#website-scope)
- [Doxygen](#website-doxygen)
- [Sources](#website-sources)

## Purpose {#website-purpose}

The layout follows the Boost pattern of a marketing home, a docs root,
and a short menu. From any inner page the header crumb reaches the
roots. Download, Getting Started, License, Jam, and the API indexes
live in the hamburger menu. Framework modules live on the Home cards
and on the documentation inventory. The Home hero offers Download,
Getting Started, Documentation, and GitHub.

Chrome has one source: `doc/pt_header.html`, `doc/pt_footer.html`, and
`doc/site.css`. The site is one link space.

The site is static. It uses no JavaScript framework, CDN, or theme
repository. `HTML_EXTRA_FILES` carries assets such as the favicon. It
does not carry page content.

Sources for generated pages are `doc/pages/*.md`. Output is
`doc/website/` as a flat tree (`HTML_OUTPUT = .`).

## Color {#website-color}

The structure accent is RGB(150, 0, 50) (`#960032`). That value is the
red used on portal bars, docs module tiles, the Home design rule, and
the primary button. It is not CSS `crimson`.

The page background is RGB(24, 24, 24) (`#181818`). Tiles are RGB(31,
31, 31) (`#1f1f1f`). Buttons are RGB(42, 42, 42) (`#2a2a2a`). The
primary button keeps the accent. Titles and bright labels are RGB(238,
238, 238) (`#eeeeee`). Body text and lists inside tiles are darker
(`#a8a8a8`). Links and visited links use that same body color, so
Doxygen blue never appears. A ghost button on hover lightens its fill
and border and stays off the accent.

Azure is the second color. It is one hue, 208 degrees, on a ladder.
Red remains the saturated structure accent. Names sit on the 400 step.
Boxes sit on 900 and 950. Code fragments and the clone box sit inset
24px from the content edge.

| Step | HSL | Hex | Role |
|---|---|---|---|
| 950 | 208deg 50% 11% | `#0E1D2A` | Clone box and code fragments |
| 700 | 208deg 32% 28% | `#30495E` | Border on clone and code only |
| 400 | 208deg 48% 54% | `#518DC2` | Class index, member names, class names |
| 300 | 208deg 16% 70% | `#A8B7C4` | Text inside the code box |

Parameters in signatures are RGB(163, 146, 116) (`#A39274`).

## Navigation {#website-navigation}

### Roots and destinations {#website-navigation-roots}

The header is a crumb. The active crumb uses the bar color. It is not
a filled square, and the bar itself carries no accent.

```
Home:          Pt*                                             ≡
Download:      Pt  ›  Get Platinum*                            ≡
Docs root:     Pt  ›  Documentation*                           ≡
Guides / API:  Pt  ›  Documentation*                           ≡
```

| Destination | Where it appears |
|---|---|
| Home | Wordmark `Pt` |
| Docs root | Crumb `Documentation` |
| Download | Crumb `Get Platinum` on the download page, hero, and menu |
| GitHub (repository) | Hero on Home and footer |
| Getting Started, License, Jam, API indexes | Menu; Getting Started also in the hero |
| Framework modules (Core, Gfx, ...) | Home cards and docs root |

### Header {#website-navigation-header}

Every page has one text bar. The crumb sits on the left and the
hamburger on the right. The bar is not a tile. It has no rule under it,
no second global bar, and no separate Documentation link on the right.

```
Home:          Pt                                              ≡
Download:      Pt  ›  Get Platinum                             ≡
Other pages:   Pt  ›  Documentation                            ≡
```

The wordmark `Pt` always opens Home (`index.html`) and uses the same
type size as the crumb (`Documentation` / `Get Platinum`). The accent
box is tight to that word. Opposite corners are cut so the remaining
edges sit flush with the letter box. The hamburger keeps the 32 by 32
pixel slot, centered in the 48 pixel bar.

`Documentation` opens the docs root (`docs.html`). On that page the
word is the current crumb: bar color, and not a link. `Get Platinum`
appears only on the download page, as the current crumb.

The hamburger opens the menu to the right under the bar. The glyph is
`≡`, 30 pixels inside the 32 by 32 slot, with the same hit area as
`.pt-nav-home`. While the menu is open the bar stays in the bar color.

The crumb (`Pt`, `›`, `Documentation` or `Get Platinum`) shares that
32 pixel height and is vertically centered. GitHub lives in the Home
hero and in the footer.

Page markers drive the crumb with CSS `:has()` and no JavaScript:
`.pt-page-home`, `.pt-page-download`, `.pt-page-docs`. Every other
page uses the Documentation crumb.

The header has no fork-me ribbon. Doxygen's tab index is off
(`DISABLE_INDEX = YES`).

### Menu {#website-navigation-menu}

The hamburger menu is the same on every page. A checkbox and CSS open
a panel under the bar at the hamburger, and dim the page below the
bar. The panel uses no JavaScript. Framework modules are absent from
the list; they already sit on Home and on the docs root.

```
Start
  Download
  Getting Started
  License

Reference
  Jam Build Tool
  Class Index
  Namespaces
  Modules
```

| Entry | Target |
|---|---|
| Download | `downloads.html` |
| Getting Started | `docs.html#getting-started` |
| License | `license.html` |
| Jam Build Tool | `docs.html#jam` |
| Class Index | `classes.html` |
| Namespaces | `namespaces.html` |
| Modules | `topics.html` (Doxygen groups) |

Home and Documentation stay out of the panel because the crumb already
reaches them. Contributing and Security stay in the Getting Started
portal as GitHub links.

The headings `START` and `REFERENCE` are labels (`div`, not links):
`#eeeeee`, 11px, weight 700, uppercase, letter-spacing 0.12em. Entries
are 14px, weight 400, `#a8a8a8`, padding 5px 12px, on the same left
edge as the labels. The panel fill is `#1f1f1f`, the border `#3a3a3a`.
Hover text is `#eeeeee` without a fill highlight. The panel is
`max-content`, `right: 16px`.

Doxygen's treeview is off (`GENERATE_TREEVIEW = NO`). A local page
outline (members, sections) stays in the API page body.

## Home {#website-home}

Home is `@mainpage` in `doc/pages/mainpage.md` and becomes
`index.html`. The hero line is **The Power in your Hands**. On a
narrow viewport the title wraps.

Doxygen's `.header` title is hidden on Home, Download, and the docs
root (`.pt-page-home`, `.pt-page-download`, `.pt-page-docs`). Those
pages supply their own titles. On other `\page`s and on API pages,
`.title` has no Doxygen banner, and `\section` sits visually under the
page title.

The hero buttons open the download page, Getting Started (the portal
group anchor on the docs root), Documentation (the docs root), and
GitHub (the repository).

```
Pt*                                                    ≡

  THE POWER IN YOUR HANDS
  Portable · Modular · Asynchronous · Standard C++ · Flexible · Open and proprietary

  Platinum (Pt) is a comprehensive C++ framework, which allows developers
  to write high-performance applications for many platforms with only one
  codebase. It provides a large amount of features and is still very easy
  to use. It integrates well into existing toolkits and frameworks.

  [ Getting Started ]    [ Documentation ]    [ GitHub ]              [ Download ]


  Modules                (one tile per module, border, one line)
  Core | System | Net | HTTP | SSL | Gfx | Forms | XmlRpc |
  Unit | Database | Lua | MCP | Reflex | Cosmo

  Design                 (four text blocks, accent rule on the left)
  Standard C++ | Asynchronous stack | Modular and non-intrusive | One codebase
```

Home is the argument. The docs root is the inventory.

The property line under the title, before the lead, contains no links.
Download sits on the right of the hero; the three ghost buttons sit on
the left. Platforms and the license have their own pages, so they stay
out of this line.

Module cards have a border and no accent bar. Each card is a title
plus one line. A click opens the module page. Chapter sublinks belong
on the docs root. Reflex has no target page, so that card has no link.

Design is not a card row. It is four text blocks with a left rule
`#960032`. There is no third bullet list.

## Download {#website-download}

Download is `\page downloads` in `doc/pages/downloads.md` and becomes
`downloads.html`. The page is a thin **Get Platinum** surface. It is
not a permanent item in the bar. The hero and the menu land here. The
page is the in-site destination; GitHub Releases is not the only hop.

Clone is the everyday path. Below it sits the current **framework
release** (`v*`). Prebuilts are not offered as downloads; Jam fetches
them with `--fetch-prebuilt`.

```
Pt  ›  Get Platinum*                                       ≡

  Get Platinum

  git clone https://github.com/Pt-Framework/Pt.git

  Source release
  [ v2.0.0-pre3 ]          all releases ->

  Framework tags start with v.
  prebuilt-* archives are dependencies, not the framework.

  A snapshot of main is also available as a source zip.

  Next:  Installing from Source
```

The primary button is the current framework tag, in `#960032`. "All
releases" is a text link to GitHub. There is no fork button. A source
zip of `main` is a text link, not the default. The version string in
the button is a line in the page, updated when the tag is cut.

Use `https://github.com/Pt-Framework/Pt/releases/latest` once a
non-prerelease exists. Until then the button points at the concrete
tag (currently `v2.0.0-pre3`). When a real release ships, change the
button text and target (`v2.0.0` to `/releases/latest`). The page, the
header, and the hero stay.

## Documentation inventory {#website-docs}

The inventory is `\page docs` in `doc/pages/docs.md` and becomes
`docs.html`. The page ID is **`docs`**. The ID `documentation` would
emit `documentation.html` and break existing `/docs.html` links.

This page is the entrance to the documentation. Three portal tiles sit
in their own row (three columns). Module tiles sit in a grid below
them, in the same tile shape. The accent bar `#960032` runs on portals
and on module tiles. The kicker **Modules** sits only above the module
tiles. Each module tile is the name as heading and links to the
chapters, with no description line.

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
  |  Command Line ...    | |  File System Access  | |  Endpoints           |
  |  Application Settings| |  Concurrency         | |  TCP Sockets         |
  |  ...                 | |  ...                 | |  UDP Sockets         |
  +----------------------+ +----------------------+ +----------------------+
  (same shape for HTTP, SSL, Gfx, Forms, XmlRpc, Unit, Database,
   Lua, MCP, Reflex, Cosmo)
```

`#` in that sketch is the accent bar. Getting Started, Reference, and
Jam Build Tool are groups on this page (links inside the tile), not
header items. Portal and module tile headings are `#eeeeee`. Links
have no underline, including on hover.

Home module tiles: border, one line, no bar. Docs module tiles: portal
shape, chapter links, no running text.

README, Contributing, and Security point at GitHub. The other targets
are generated `\page`s.

## API pages {#website-api}

An API page has two levels and no rules between them. The site bar
shows the Documentation crumb. Under it sits **one** context line, on
class, namespace, member, file, group, and index pages:

- Left, inventory: Class Index · Namespaces · Modules
- Right, Doxygen navpath (location), in the same flex row (`#top`)

`DISABLE_INDEX = YES` turns off Doxygen tabs. Navpath stays on. The
API links are HTML in the header, not Doxygen's index. Doxygen's
`.header` is the page title, not a third bar. Title and module parent
link are centered. Below them, left-aligned, come the include and then
the brief. `.summary` (Public Member Functions and the rest) is off;
those headings live in the member block. Member names use the azure of
class-index names (`#518DC2`). Briefs sit under the signature in the
classic two-column grid, not as member cards. Member details use fill
`#1f1f1f` without a border, 8px between blocks. Azure is a name color
only.

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

Page markers `.pt-page-home`, `.pt-page-download`, and `.pt-page-docs`
turn the context line off on those three pages. Other `\page`s without
class or group markup leave it off as well.

Navpath uses the same font size as the inventory, `#a8a8a8`, separator
`›` at `1em` of the line, last crumb a little lighter. Inventory items
are separated by middots: Class Index · Namespaces · Modules. Doxygen
breadcrumb graphics are unused. The path has no `#960032` bar; that
bar stays on portals and docs module tiles.

### Destinations from an API type {#website-api-destinations}

```
Pt::PaintSurface
    Documentation   ->  docs root (docs.html)
    Class Index     ->  A-Z (classes.html)
    Gfx in navpath  ->  Gfx group / Pt-Gfx-Page
    Menu            ->  Download, Getting Started, Jam, API indexes
    Pt              ->  Home
```

Doxygen emits the pages (`classes.html`, `classPt_1_1String.html`, and
the rest). The path to them is the API line plus the menu.

## Chrome and layout {#website-chrome}

`doc/site.css` is the overlay (`HTML_EXTRA_STYLESHEET`). Doxygen's
default stylesheet remains. The site bar is sticky. On API pages the
context line under it is sticky as well. Hover is CSS only. Chrome has
no horizontal rules.

The footer is Copyright · License · GitHub. The favicon arrives
through `HTML_EXTRA_FILES`. Logo artwork comes later.

From 800px downward, portals, Design, and three-column grids stack.
The hero title shrinks and wraps.

| Role | Where | Shape |
|---|---|---|
| Site bar | everywhere | Crumb left, hamburger right (32x32 like Pt); no accent |
| Context line | API pages | Inventory left, navpath right |
| Properties | Home under the hero | Line, no links, no border |
| Modules (Home) | Home | Border, title plus one line, no bar |
| Design | Home | Text, left rule `#960032`, no border |
| Portals | Docs root | Border, accent bar on top, link lists |
| Modules (Docs) | Docs root | Same shape as portals; name plus chapters |

The accent bar belongs on docs portals and docs module tiles. Design
is never a card. The header is a bar, never a tile.

## Scope {#website-scope}

The site has no Features area, About item, or Community page. It has
no fork ribbon. Download is absent from the site bar. Doxygen's
treeview is not the main navigation. Search is off (`SEARCHENGINE =
NO`) and, when it arrives, sits at the bottom of the menu. Artwork
(logo, hamburger, hero) is later work.

The footer is Copyright, License, GitHub.

Contributing and Security are links in the Getting Started portal.
They are absent from the header and from the menu.

## Doxygen {#website-doxygen}

One generated site, one output directory (`OUTPUT_DIRECTORY =
./website`, `HTML_OUTPUT = .`).

| Page | Mechanism | File |
|---|---|---|
| Home | `USE_MDFILE_AS_MAINPAGE = pages/mainpage.md` | `index.html` |
| Download | `\page downloads` | `downloads.html` |
| Docs root | `\page docs` | `docs.html` |
| Installing, Platforms, License, Jam, modules | `\page`s in `doc/pages/` | `<id>.html` |
| Header / footer | `pt_header.html` / `pt_footer.html` | Bar, menu, context line, `$navpath` |
| CSS | `HTML_EXTRA_STYLESHEET = site.css` | Overlay; `HTML_STYLESHEET` stays unused |
| Extra files | Favicon (logo later) | |
| Index tabs | `DISABLE_INDEX = YES` | |
| Treeview | `GENERATE_TREEVIEW = NO` | Menu is own HTML |
| Search | `SEARCHENGINE = NO` | |

`docs.html` is the docs root. `index.html` is the mainpage.
`downloads.html` is Get Platinum.

## Sources {#website-sources}

| Source | Role |
|---|---|
| `doc/pages/mainpage.md` | Home |
| `doc/pages/docs.md` | Docs root |
| `doc/pages/downloads.md` | Get Platinum |
| `doc/pages/license.md` | License |
| `doc/pages/platforms.md` | Platforms |
| `doc/pt_header.html` | Bar, menu, context line |
| `doc/pt_footer.html` | Footer |
| `doc/site.css` | Overlay, accent `#960032` |
