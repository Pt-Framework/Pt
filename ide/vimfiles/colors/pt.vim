" Maintainer:	Henrique C. Alves (hcarvalhoalves@gmail.com)
" Version:      1.0
" Last Change:	September 25 2008

set background=dark

hi clear

if exists("syntax_on")
  syntax reset
endif

let colors_name = "mustang"

" Vim >= 7.0 specific colors
if version >= 700
  hi CursorLine guibg=#2d2d2d ctermbg=236
  hi CursorColumn guibg=#2d2d2d ctermbg=236
  hi MatchParen guifg=#d0ffc0 guibg=#2f2f2f gui=bold ctermfg=157 ctermbg=237 cterm=bold
  hi Pmenu 		guifg=#ffffff guibg=#444444 ctermfg=255 ctermbg=238
  hi PmenuSel 	guifg=#000000 guibg=#b1d631 ctermfg=0 ctermbg=148
endif

" General colors
hi Cursor 	guifg=NONE    guibg=#626262 gui=none ctermbg=241
hi Normal 	guifg=#c0c0c0 guibg=#202020 gui=none ctermfg=253 ctermbg=234
hi NonText 	guifg=#c0c0c0 guibg=#202020 gui=none ctermfg=244 ctermbg=235
hi LineNr 	guifg=#808080 guibg=#000000 gui=none ctermfg=244 ctermbg=232
hi StatusLine 	guifg=#d3d3d5 guibg=#444444 gui=italic ctermfg=253 ctermbg=238 cterm=italic
hi StatusLineNC guifg=#939395 guibg=#444444 gui=none ctermfg=246 ctermbg=238
hi VertSplit 	guifg=#444444 guibg=#444444 gui=none ctermfg=238 ctermbg=238
hi Folded 	guibg=#384048 guifg=#a0a8b0 gui=none ctermbg=4 ctermfg=248
hi Title	guifg=#f6f3e8 guibg=NONE	gui=bold ctermfg=254 cterm=bold
hi Visual	guifg=#faf4c6 guibg=#3c414c gui=none ctermfg=254 ctermbg=4
hi SpecialKey	guifg=#808080 guibg=#343434 gui=none ctermfg=244 ctermbg=236

" Syntax highlighting
hi Comment guifg=#808080 gui=none ctermfg=244
hi Todo    guifg=#8f8f8f guibg=#e6ea50 gui=italic ctermfg=245

hi PreProc  guifg=#87DF8C gui=none ctermfg=230

hi String   guifg=#DF8787 gui=none ctermfg=148
hi Character   guifg=#DF9987 gui=none ctermfg=148
hi Constant guifg=#87afdf gui=none  ctermfg=208
hi Number   guifg=#87afdf gui=none ctermfg=208
hi Special  guifg=#DFD487 gui=none ctermfg=208

hi Identifier guifg=orange gui=none ctermfg=148
hi link Function Normal

hi Type         guifg=white gui=none ctermfg=103
hi Statement    guifg=white gui=none ctermfg=103
hi Identifier   guifg=white gui=none
hi Statement    guifg=white gui=none
hi Structure    guifg=white gui=none
hi Keyword      guifg=white gui=none
hi Boolean      guifg=white gui=none ctermfg=148

"hi TypeModifier guifg=#87afdf gui=none
hi StorageClass guifg=#87afdf gui=none
hi cType guifg=#87afdf gui=none
hi cppType guifg=#87afdf gui=none