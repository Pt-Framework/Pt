" Maintainer:	Henrique C. Alves (hcarvalhoalves@gmail.com)
" Version:      1.0
" Last Change:	September 25 2008

set background=dark

hi clear

if exists("syntax_on")
  syntax reset
endif

let colors_name = "pt"

" Vim >= 7.0 specific colors
if version >= 700
  hi CursorLine guibg=#131313 ctermbg=236
  hi CursorColumn guibg=#2d2d2d ctermbg=236
  hi MatchParen guifg=yellow guibg=#000000 gui=bold ctermfg=157 ctermbg=237 cterm=bold
  hi Pmenu 		guifg=#ffffff guibg=#444444 ctermfg=255 ctermbg=238
  hi PmenuSel 	guifg=#000000 guibg=#b1d631 ctermfg=0 ctermbg=148
endif

" General colors
hi cursor       guifg=black   guibg=yellow gui=none ctermbg=241
hi Normal       guifg=#c0c0c0 guibg=#000000 gui=none ctermfg=253 ctermbg=234
hi NonText      guifg=#000000 guibg=#000000 gui=none ctermfg=244 ctermbg=235
hi LineNr       guifg=#808080 guibg=#151515 gui=none ctermfg=244 ctermbg=232
hi StatusLine   guifg=#dddddd guibg=#444444 gui=none ctermfg=253 ctermbg=238 cterm=none
hi StatusLineNC guifg=#959595 guibg=#444444 gui=none ctermfg=246 ctermbg=238
hi VertSplit    guifg=#000000 guibg=#444444 gui=none ctermfg=238 ctermbg=238
hi Folded       guibg=#384048 guifg=#a0a8b0 gui=none ctermbg=4 ctermfg=248
hi FoldColumn   guibg=#151515 guifg=darkyellow 
hi Title        guifg=#f6f3e8 guibg=NONE	gui=bold ctermfg=254 cterm=bold
hi Visual       guifg=#ffffff guibg=#3c414c gui=none ctermfg=254 ctermbg=4
hi SpecialKey   guifg=#808080 guibg=#343434 gui=none ctermfg=244 ctermbg=236
hi IncSearch    guifg=#3c414c guibg=#faf4c6
hi Search       guifg=cyan guibg=NONE
hi Title        guifg=white guibg=NONE

" Taglist plugin
hi MyTagListFileName guifg=white guibg=NONE gui=bold

" highlight groups
"hi CursorIM
"hi Directory
"hi DiffAdd
"hi DiffChange
"hi DiffDelete
"hi DiffText
"hi ErrorMsg
"hi LineNr
"hi ModeMsg	guifg=goldenrod
"hi MoreMsg	guifg=SeaGreen
"hi NonText	guifg=LightBlue guibg=grey30
"hi Question	guifg=springgreen
"hi Search	guibg=peru guifg=wheat
"hi SpecialKey	guifg=yellowgreen
"hi VisualNOS
"hi WarningMsg	guifg=salmon
"hi WildMenu
"hi Menu
"hi Scrollbar
"hi Tooltip

" Syntax highlighting
hi Comment guifg=#808080 guibg=NONE gui=none ctermfg=244
hi Todo    guifg=#8f8f8f guibg=#e6ea50 gui=italic ctermfg=245


hi String    guifg=#DF8787 gui=none ctermfg=148
hi Character guifg=#DF9987 gui=none ctermfg=148
hi Constant  guifg=#87afdf gui=none  ctermfg=208
hi Number    guifg=#87afdf gui=none ctermfg=208
hi Float     guifg=#87DF99 gui=none 

hi Identifier guifg=orange gui=none ctermfg=148
hi link Function Normal

hi Special      guifg=white gui=none ctermfg=208
hi Type         guifg=white gui=none ctermfg=103
hi Statement    guifg=white gui=none ctermfg=103
hi Identifier   guifg=white gui=none
hi Statement    guifg=white gui=none
hi Structure    guifg=white gui=none
hi Keyword      guifg=white gui=none
hi Boolean      guifg=white gui=none ctermfg=148

"hi TypeModifier guifg=#87afdf gui=none
hi PreProc       guifg=#87afdf gui=none ctermfg=230
hi StorageClass  guifg=#87afdf gui=none
hi cType         guifg=#87afdf gui=none
hi cppType       guifg=#87afdf gui=none
