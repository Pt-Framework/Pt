" Vimball Archiver by Charles E. Campbell, Jr., Ph.D.
UseVimball
finish
autoload/tagbar.vim	[[[1
3439
" ============================================================================
" File:        tagbar.vim
" Description: List the current file's tags in a sidebar, ordered by class etc
" Author:      Jan Larres <jan@majutsushi.net>
" Licence:     Vim licence
" Website:     http://majutsushi.github.com/tagbar/
" Version:     2.4
" Note:        This plugin was heavily inspired by the 'Taglist' plugin by
"              Yegappan Lakshmanan and uses a small amount of code from it.
"
" Original taglist copyright notice:
"              Permission is hereby granted to use and distribute this code,
"              with or without modifications, provided that this copyright
"              notice is copied with it. Like anything else that's free,
"              taglist.vim is provided *as is* and comes with no warranty of
"              any kind, either expressed or implied. In no event will the
"              copyright holder be liable for any damamges resulting from the
"              use of this software.
" ============================================================================

scriptencoding utf-8

" Initialization {{{1

" Basic init {{{2

if !exists('g:tagbar_ctags_bin')
    let ctagsbins  = []
    let ctagsbins += ['ctags-exuberant'] " Debian
    let ctagsbins += ['exuberant-ctags']
    let ctagsbins += ['exctags'] " FreeBSD, NetBSD
    let ctagsbins += ['/usr/local/bin/ctags'] " Homebrew
    let ctagsbins += ['/opt/local/bin/ctags'] " Macports
    let ctagsbins += ['ectags'] " OpenBSD
    let ctagsbins += ['ctags']
    let ctagsbins += ['ctags.exe']
    let ctagsbins += ['tags']
    for ctags in ctagsbins
        if executable(ctags)
            let g:tagbar_ctags_bin = ctags
            break
        endif
    endfor
    if !exists('g:tagbar_ctags_bin')
        echomsg 'Tagbar: Exuberant ctags not found, skipping plugin'
        finish
    endif
else
    " reset 'wildignore' temporarily in case *.exe is included in it
    let wildignore_save = &wildignore
    set wildignore&

    let g:tagbar_ctags_bin = expand(g:tagbar_ctags_bin)

    let &wildignore = wildignore_save

    if !executable(g:tagbar_ctags_bin)
        echomsg "Tagbar: Exuberant ctags not found at " .
              \ "'" . g:tagbar_ctags_bin . "', " .
              \ "skipping plugin"
        finish
    endif
endif

redir => s:ftype_out
silent filetype
redir END
if s:ftype_out !~# 'detection:ON'
    echomsg 'Tagbar: Filetype detection is turned off, skipping plugin'
    unlet s:ftype_out
    finish
endif
unlet s:ftype_out

let s:icon_closed = g:tagbar_iconchars[0]
let s:icon_open   = g:tagbar_iconchars[1]

let s:type_init_done      = 0
let s:autocommands_done   = 0
let s:checked_ctags       = 0
let s:checked_ctags_types = 0
let s:ctags_types         = {}
let s:window_expanded     = 0


let s:access_symbols = {
    \ 'public'    : '+',
    \ 'protected' : '#',
    \ 'private'   : '-'
\ }

let g:loaded_tagbar = 1

let s:last_highlight_tline = 0
let s:debug = 0
let s:debug_file = ''

" s:Init() {{{2
function! s:Init()
    if !s:checked_ctags
        if !s:CheckForExCtags()
            return 0
        endif
    endif

    if !s:checked_ctags_types
        call s:GetSupportedFiletypes()
    endif

    if !s:type_init_done
        call s:InitTypes()
    endif

    if !s:autocommands_done
        call s:CreateAutocommands()
        doautocmd CursorHold
    endif

    return 1
endfunction

" s:InitTypes() {{{2
function! s:InitTypes()
    call s:LogDebugMessage('Initializing types')

    let s:known_types = {}

    " Ant {{{3
    let type_ant = s:TypeInfo.New()
    let type_ant.ctagstype = 'ant'
    let type_ant.kinds     = [
        \ {'short' : 'p', 'long' : 'projects', 'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'targets',  'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.ant = type_ant
    " Asm {{{3
    let type_asm = s:TypeInfo.New()
    let type_asm.ctagstype = 'asm'
    let type_asm.kinds     = [
        \ {'short' : 'm', 'long' : 'macros',  'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'types',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'd', 'long' : 'defines', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'l', 'long' : 'labels',  'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.asm = type_asm
    " ASP {{{3
    let type_aspvbs = s:TypeInfo.New()
    let type_aspvbs.ctagstype = 'asp'
    let type_aspvbs.kinds     = [
        \ {'short' : 'd', 'long' : 'constants',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'classes',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',   'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'subroutines', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables',   'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.aspvbs = type_aspvbs
    " Awk {{{3
    let type_awk = s:TypeInfo.New()
    let type_awk.ctagstype = 'awk'
    let type_awk.kinds     = [
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.awk = type_awk
    " Basic {{{3
    let type_basic = s:TypeInfo.New()
    let type_basic.ctagstype = 'basic'
    let type_basic.kinds     = [
        \ {'short' : 'c', 'long' : 'constants',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'g', 'long' : 'enumerations', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'l', 'long' : 'labels',       'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'types',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables',    'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.basic = type_basic
    " BETA {{{3
    let type_beta = s:TypeInfo.New()
    let type_beta.ctagstype = 'beta'
    let type_beta.kinds     = [
        \ {'short' : 'f', 'long' : 'fragments', 'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'slots',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'patterns',  'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.beta = type_beta
    " C {{{3
    let type_c = s:TypeInfo.New()
    let type_c.ctagstype = 'c'
    let type_c.kinds     = [
        \ {'short' : 'd', 'long' : 'macros',      'fold' : 1, 'stl' : 0},
        \ {'short' : 'p', 'long' : 'prototypes',  'fold' : 1, 'stl' : 0},
        \ {'short' : 'g', 'long' : 'enums',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'enumerators', 'fold' : 0, 'stl' : 0},
        \ {'short' : 't', 'long' : 'typedefs',    'fold' : 0, 'stl' : 0},
        \ {'short' : 's', 'long' : 'structs',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'u', 'long' : 'unions',      'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'members',     'fold' : 0, 'stl' : 0},
        \ {'short' : 'v', 'long' : 'variables',   'fold' : 0, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'functions',   'fold' : 0, 'stl' : 1}
    \ ]
    let type_c.sro        = '::'
    let type_c.kind2scope = {
        \ 'g' : 'enum',
        \ 's' : 'struct',
        \ 'u' : 'union'
    \ }
    let type_c.scope2kind = {
        \ 'enum'   : 'g',
        \ 'struct' : 's',
        \ 'union'  : 'u'
    \ }
    let s:known_types.c = type_c
    " C++ {{{3
    let type_cpp = s:TypeInfo.New()
    let type_cpp.ctagstype = 'c++'
    let type_cpp.kinds     = [
        \ {'short' : 'd', 'long' : 'macros',      'fold' : 1, 'stl' : 0},
        \ {'short' : 'p', 'long' : 'prototypes',  'fold' : 1, 'stl' : 0},
        \ {'short' : 'g', 'long' : 'enums',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'enumerators', 'fold' : 0, 'stl' : 0},
        \ {'short' : 't', 'long' : 'typedefs',    'fold' : 0, 'stl' : 0},
        \ {'short' : 'n', 'long' : 'namespaces',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'classes',     'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'structs',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'u', 'long' : 'unions',      'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'members',     'fold' : 0, 'stl' : 0},
        \ {'short' : 'v', 'long' : 'variables',   'fold' : 0, 'stl' : 0}
    \ ]
    let type_cpp.sro        = '::'
    let type_cpp.kind2scope = {
        \ 'g' : 'enum',
        \ 'n' : 'namespace',
        \ 'c' : 'class',
        \ 's' : 'struct',
        \ 'u' : 'union'
    \ }
    let type_cpp.scope2kind = {
        \ 'enum'      : 'g',
        \ 'namespace' : 'n',
        \ 'class'     : 'c',
        \ 'struct'    : 's',
        \ 'union'     : 'u'
    \ }
    let s:known_types.cpp = type_cpp
    " C# {{{3
    let type_cs = s:TypeInfo.New()
    let type_cs.ctagstype = 'c#'
    let type_cs.kinds     = [
        \ {'short' : 'd', 'long' : 'macros',      'fold' : 1, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'fields',      'fold' : 0, 'stl' : 1},
        \ {'short' : 'g', 'long' : 'enums',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'enumerators', 'fold' : 0, 'stl' : 0},
        \ {'short' : 't', 'long' : 'typedefs',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'n', 'long' : 'namespaces',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'i', 'long' : 'interfaces',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'classes',     'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'structs',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'E', 'long' : 'events',      'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'methods',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'properties',  'fold' : 0, 'stl' : 1}
    \ ]
    let type_cs.sro        = '.'
    let type_cs.kind2scope = {
        \ 'n' : 'namespace',
        \ 'i' : 'interface',
        \ 'c' : 'class',
        \ 's' : 'struct',
        \ 'g' : 'enum'
    \ }
    let type_cs.scope2kind = {
        \ 'namespace' : 'n',
        \ 'interface' : 'i',
        \ 'class'     : 'c',
        \ 'struct'    : 's',
        \ 'enum'      : 'g'
    \ }
    let s:known_types.cs = type_cs
    " COBOL {{{3
    let type_cobol = s:TypeInfo.New()
    let type_cobol.ctagstype = 'cobol'
    let type_cobol.kinds     = [
        \ {'short' : 'd', 'long' : 'data items',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'file descriptions', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'g', 'long' : 'group items',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'paragraphs',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'P', 'long' : 'program ids',       'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'sections',          'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.cobol = type_cobol
    " DOS Batch {{{3
    let type_dosbatch = s:TypeInfo.New()
    let type_dosbatch.ctagstype = 'dosbatch'
    let type_dosbatch.kinds     = [
        \ {'short' : 'l', 'long' : 'labels',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.dosbatch = type_dosbatch
    " Eiffel {{{3
    let type_eiffel = s:TypeInfo.New()
    let type_eiffel.ctagstype = 'eiffel'
    let type_eiffel.kinds     = [
        \ {'short' : 'c', 'long' : 'classes',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'features', 'fold' : 0, 'stl' : 1}
    \ ]
    let type_eiffel.sro        = '.' " Not sure, is nesting even possible?
    let type_eiffel.kind2scope = {
        \ 'c' : 'class',
        \ 'f' : 'feature'
    \ }
    let type_eiffel.scope2kind = {
        \ 'class'   : 'c',
        \ 'feature' : 'f'
    \ }
    let s:known_types.eiffel = type_eiffel
    " Erlang {{{3
    let type_erlang = s:TypeInfo.New()
    let type_erlang.ctagstype = 'erlang'
    let type_erlang.kinds     = [
        \ {'short' : 'm', 'long' : 'modules',            'fold' : 0, 'stl' : 1},
        \ {'short' : 'd', 'long' : 'macro definitions',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'r', 'long' : 'record definitions', 'fold' : 0, 'stl' : 1}
    \ ]
    let type_erlang.sro        = '.' " Not sure, is nesting even possible?
    let type_erlang.kind2scope = {
        \ 'm' : 'module'
    \ }
    let type_erlang.scope2kind = {
        \ 'module' : 'm'
    \ }
    let s:known_types.erlang = type_erlang
    " Flex {{{3
    " Vim doesn't support Flex out of the box, this is based on rough
    " guesses and probably requires
    " http://www.vim.org/scripts/script.php?script_id=2909
    " Improvements welcome!
    let type_mxml = s:TypeInfo.New()
    let type_mxml.ctagstype = 'flex'
    let type_mxml.kinds     = [
        \ {'short' : 'v', 'long' : 'global variables', 'fold' : 0, 'stl' : 0},
        \ {'short' : 'c', 'long' : 'classes',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'methods',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'properties',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'x', 'long' : 'mxtags',           'fold' : 0, 'stl' : 0}
    \ ]
    let type_mxml.sro        = '.'
    let type_mxml.kind2scope = {
        \ 'c' : 'class'
    \ }
    let type_mxml.scope2kind = {
        \ 'class' : 'c'
    \ }
    let s:known_types.mxml = type_mxml
    " Fortran {{{3
    let type_fortran = s:TypeInfo.New()
    let type_fortran.ctagstype = 'fortran'
    let type_fortran.kinds     = [
        \ {'short' : 'm', 'long' : 'modules',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'programs',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'k', 'long' : 'components', 'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'derived types and structures', 'fold' : 0,
         \ 'stl' : 1},
        \ {'short' : 'c', 'long' : 'common blocks', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'b', 'long' : 'block data',    'fold' : 0, 'stl' : 0},
        \ {'short' : 'e', 'long' : 'entry points',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',     'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'subroutines',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'l', 'long' : 'labels',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'n', 'long' : 'namelists',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables',     'fold' : 0, 'stl' : 0}
    \ ]
    let type_fortran.sro        = '.' " Not sure, is nesting even possible?
    let type_fortran.kind2scope = {
        \ 'm' : 'module',
        \ 'p' : 'program',
        \ 'f' : 'function',
        \ 's' : 'subroutine'
    \ }
    let type_fortran.scope2kind = {
        \ 'module'     : 'm',
        \ 'program'    : 'p',
        \ 'function'   : 'f',
        \ 'subroutine' : 's'
    \ }
    let s:known_types.fortran = type_fortran
    " HTML {{{3
    let type_html = s:TypeInfo.New()
    let type_html.ctagstype = 'html'
    let type_html.kinds     = [
        \ {'short' : 'f', 'long' : 'JavaScript funtions', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'a', 'long' : 'named anchors',       'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.html = type_html
    " Java {{{3
    let type_java = s:TypeInfo.New()
    let type_java.ctagstype = 'java'
    let type_java.kinds     = [
        \ {'short' : 'p', 'long' : 'packages',       'fold' : 1, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'fields',         'fold' : 0, 'stl' : 0},
        \ {'short' : 'g', 'long' : 'enum types',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'enum constants', 'fold' : 0, 'stl' : 0},
        \ {'short' : 'i', 'long' : 'interfaces',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'classes',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'methods',        'fold' : 0, 'stl' : 1}
    \ ]
    let type_java.sro        = '.'
    let type_java.kind2scope = {
        \ 'g' : 'enum',
        \ 'i' : 'interface',
        \ 'c' : 'class'
    \ }
    let type_java.scope2kind = {
        \ 'enum'      : 'g',
        \ 'interface' : 'i',
        \ 'class'     : 'c'
    \ }
    let s:known_types.java = type_java
    " JavaScript {{{3
    " JavaScript is weird -- it does have scopes, but ctags doesn't seem to
    " properly generate the information for them, instead it simply uses the
    " complete name. So ctags has to be fixed before I can do anything here.
    " Alternatively jsctags/doctorjs will be used if available.
    let type_javascript = s:TypeInfo.New()
    let type_javascript.ctagstype = 'javascript'
    let jsctags = s:CheckFTCtags('jsctags', 'javascript')
    if jsctags != ''
        let type_javascript.kinds = [
            \ {'short' : 'v', 'long' : 'variables', 'fold' : 0, 'stl' : 0},
            \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1}
        \ ]
        let type_javascript.sro        = '.'
        let type_javascript.kind2scope = {
            \ 'v' : 'namespace',
            \ 'f' : 'namespace'
        \ }
        let type_javascript.scope2kind = {
            \ 'namespace' : 'v'
        \ }
        let type_javascript.ctagsbin   = jsctags
        let type_javascript.ctagsargs  = '-f -'
    else
        let type_javascript.kinds = [
            \ {'short' : 'v', 'long' : 'global variables', 'fold' : 0, 'stl' : 0},
            \ {'short' : 'c', 'long' : 'classes',          'fold' : 0, 'stl' : 1},
            \ {'short' : 'p', 'long' : 'properties',       'fold' : 0, 'stl' : 0},
            \ {'short' : 'm', 'long' : 'methods',          'fold' : 0, 'stl' : 1},
            \ {'short' : 'f', 'long' : 'functions',        'fold' : 0, 'stl' : 1}
        \ ]
    endif
    let s:known_types.javascript = type_javascript
    " Lisp {{{3
    let type_lisp = s:TypeInfo.New()
    let type_lisp.ctagstype = 'lisp'
    let type_lisp.kinds     = [
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.lisp = type_lisp
    " Lua {{{3
    let type_lua = s:TypeInfo.New()
    let type_lua.ctagstype = 'lua'
    let type_lua.kinds     = [
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.lua = type_lua
    " Make {{{3
    let type_make = s:TypeInfo.New()
    let type_make.ctagstype = 'make'
    let type_make.kinds     = [
        \ {'short' : 'm', 'long' : 'macros', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.make = type_make
    " Matlab {{{3
    let type_matlab = s:TypeInfo.New()
    let type_matlab.ctagstype = 'matlab'
    let type_matlab.kinds     = [
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.matlab = type_matlab
    " Ocaml {{{3
    let type_ocaml = s:TypeInfo.New()
    let type_ocaml.ctagstype = 'ocaml'
    let type_ocaml.kinds     = [
        \ {'short' : 'M', 'long' : 'modules or functors', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'global variables',    'fold' : 0, 'stl' : 0},
        \ {'short' : 'c', 'long' : 'classes',             'fold' : 0, 'stl' : 1},
        \ {'short' : 'C', 'long' : 'constructors',        'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'methods',             'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'exceptions',          'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'type names',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'r', 'long' : 'structure fields',    'fold' : 0, 'stl' : 0}
    \ ]
    let type_ocaml.sro        = '.' " Not sure, is nesting even possible?
    let type_ocaml.kind2scope = {
        \ 'M' : 'Module',
        \ 'c' : 'class',
        \ 't' : 'type'
    \ }
    let type_ocaml.scope2kind = {
        \ 'Module' : 'M',
        \ 'class'  : 'c',
        \ 'type'   : 't'
    \ }
    let s:known_types.ocaml = type_ocaml
    " Pascal {{{3
    let type_pascal = s:TypeInfo.New()
    let type_pascal.ctagstype = 'pascal'
    let type_pascal.kinds     = [
        \ {'short' : 'f', 'long' : 'functions',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'procedures', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.pascal = type_pascal
    " Perl {{{3
    let type_perl = s:TypeInfo.New()
    let type_perl.ctagstype = 'perl'
    let type_perl.kinds     = [
        \ {'short' : 'p', 'long' : 'packages',    'fold' : 1, 'stl' : 0},
        \ {'short' : 'c', 'long' : 'constants',   'fold' : 0, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'formats',     'fold' : 0, 'stl' : 0},
        \ {'short' : 'l', 'long' : 'labels',      'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'subroutines', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.perl = type_perl
    " PHP {{{3
    let type_php = s:TypeInfo.New()
    let type_php.ctagstype = 'php'
    let type_php.kinds     = [
        \ {'short' : 'i', 'long' : 'interfaces',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'classes',              'fold' : 0, 'stl' : 1},
        \ {'short' : 'd', 'long' : 'constant definitions', 'fold' : 0, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'functions',            'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables',            'fold' : 0, 'stl' : 0},
        \ {'short' : 'j', 'long' : 'javascript functions', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.php = type_php
    " Python {{{3
    let type_python = s:TypeInfo.New()
    let type_python.ctagstype = 'python'
    let type_python.kinds     = [
        \ {'short' : 'i', 'long' : 'imports',   'fold' : 1, 'stl' : 0},
        \ {'short' : 'c', 'long' : 'classes',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'members',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables', 'fold' : 0, 'stl' : 0}
    \ ]
    let type_python.sro        = '.'
    let type_python.kind2scope = {
        \ 'c' : 'class',
        \ 'f' : 'function',
        \ 'm' : 'function'
    \ }
    let type_python.scope2kind = {
        \ 'class'    : 'c',
        \ 'function' : 'f'
    \ }
    let s:known_types.python = type_python
    " REXX {{{3
    let type_rexx = s:TypeInfo.New()
    let type_rexx.ctagstype = 'rexx'
    let type_rexx.kinds     = [
        \ {'short' : 's', 'long' : 'subroutines', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.rexx = type_rexx
    " Ruby {{{3
    let type_ruby = s:TypeInfo.New()
    let type_ruby.ctagstype = 'ruby'
    let type_ruby.kinds     = [
        \ {'short' : 'm', 'long' : 'modules',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'classes',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'methods',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'F', 'long' : 'singleton methods', 'fold' : 0, 'stl' : 1}
    \ ]
    let type_ruby.sro        = '.'
    let type_ruby.kind2scope = {
        \ 'c' : 'class',
        \ 'm' : 'class'
    \ }
    let type_ruby.scope2kind = {
        \ 'class' : 'c'
    \ }
    let s:known_types.ruby = type_ruby
    " Scheme {{{3
    let type_scheme = s:TypeInfo.New()
    let type_scheme.ctagstype = 'scheme'
    let type_scheme.kinds     = [
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'sets',      'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.scheme = type_scheme
    " Shell script {{{3
    let type_sh = s:TypeInfo.New()
    let type_sh.ctagstype = 'sh'
    let type_sh.kinds     = [
        \ {'short' : 'f', 'long' : 'functions', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.sh = type_sh
    let s:known_types.csh = type_sh
    let s:known_types.zsh = type_sh
    " SLang {{{3
    let type_slang = s:TypeInfo.New()
    let type_slang.ctagstype = 'slang'
    let type_slang.kinds     = [
        \ {'short' : 'n', 'long' : 'namespaces', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',  'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.slang = type_slang
    " SML {{{3
    let type_sml = s:TypeInfo.New()
    let type_sml.ctagstype = 'sml'
    let type_sml.kinds     = [
        \ {'short' : 'e', 'long' : 'exception declarations', 'fold' : 0, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'function definitions',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'functor definitions',    'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'signature declarations', 'fold' : 0, 'stl' : 0},
        \ {'short' : 'r', 'long' : 'structure declarations', 'fold' : 0, 'stl' : 0},
        \ {'short' : 't', 'long' : 'type definitions',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'value bindings',         'fold' : 0, 'stl' : 0}
    \ ]
    let s:known_types.sml = type_sml
    " SQL {{{3
    " The SQL ctags parser seems to be buggy for me, so this just uses the
    " normal kinds even though scopes should be available. Improvements
    " welcome!
    let type_sql = s:TypeInfo.New()
    let type_sql.ctagstype = 'sql'
    let type_sql.kinds     = [
        \ {'short' : 'P', 'long' : 'packages',               'fold' : 1, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'cursors',                'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',              'fold' : 0, 'stl' : 1},
        \ {'short' : 'F', 'long' : 'record fields',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'L', 'long' : 'block label',            'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'procedures',             'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'subtypes',               'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'tables',                 'fold' : 0, 'stl' : 1},
        \ {'short' : 'T', 'long' : 'triggers',               'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables',              'fold' : 0, 'stl' : 1},
        \ {'short' : 'i', 'long' : 'indexes',                'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'events',                 'fold' : 0, 'stl' : 1},
        \ {'short' : 'U', 'long' : 'publications',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'R', 'long' : 'services',               'fold' : 0, 'stl' : 1},
        \ {'short' : 'D', 'long' : 'domains',                'fold' : 0, 'stl' : 1},
        \ {'short' : 'V', 'long' : 'views',                  'fold' : 0, 'stl' : 1},
        \ {'short' : 'n', 'long' : 'synonyms',               'fold' : 0, 'stl' : 1},
        \ {'short' : 'x', 'long' : 'MobiLink Table Scripts', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'y', 'long' : 'MobiLink Conn Scripts',  'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.sql = type_sql
    " Tcl {{{3
    let type_tcl = s:TypeInfo.New()
    let type_tcl.ctagstype = 'tcl'
    let type_tcl.kinds     = [
        \ {'short' : 'c', 'long' : 'classes',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'methods',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'procedures', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.tcl = type_tcl
    " LaTeX {{{3
    let type_tex = s:TypeInfo.New()
    let type_tex.ctagstype = 'tex'
    let type_tex.kinds     = [
        \ {'short' : 'i', 'long' : 'includes',       'fold' : 1, 'stl' : 0},
        \ {'short' : 'p', 'long' : 'parts',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'chapters',       'fold' : 0, 'stl' : 1},
        \ {'short' : 's', 'long' : 'sections',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'u', 'long' : 'subsections',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'b', 'long' : 'subsubsections', 'fold' : 0, 'stl' : 1},
        \ {'short' : 'P', 'long' : 'paragraphs',     'fold' : 0, 'stl' : 0},
        \ {'short' : 'G', 'long' : 'subparagraphs',  'fold' : 0, 'stl' : 0},
        \ {'short' : 'l', 'long' : 'labels',         'fold' : 0, 'stl' : 0}
    \ ]
    let type_tex.sro        = '""'
    let type_tex.kind2scope = {
        \ 'p' : 'part',
        \ 'c' : 'chapter',
        \ 's' : 'section',
        \ 'u' : 'subsection',
        \ 'b' : 'subsubsection'
    \ }
    let type_tex.scope2kind = {
        \ 'part'          : 'p',
        \ 'chapter'       : 'c',
        \ 'section'       : 's',
        \ 'subsection'    : 'u',
        \ 'subsubsection' : 'b'
    \ }
    let type_tex.sort = 0
    let s:known_types.tex = type_tex
    " Vala {{{3
    " Vala is supported by the ctags fork provided by Anjuta, so only add the
    " type if the fork is used to prevent error messages otherwise
    if has_key(s:ctags_types, 'vala') || executable('anjuta-tags')
        let type_vala = s:TypeInfo.New()
        let type_vala.ctagstype = 'vala'
        let type_vala.kinds     = [
            \ {'short' : 'e', 'long' : 'Enumerations',       'fold' : 0, 'stl' : 1},
            \ {'short' : 'v', 'long' : 'Enumeration values', 'fold' : 0, 'stl' : 0},
            \ {'short' : 's', 'long' : 'Structures',         'fold' : 0, 'stl' : 1},
            \ {'short' : 'i', 'long' : 'Interfaces',         'fold' : 0, 'stl' : 1},
            \ {'short' : 'd', 'long' : 'Delegates',          'fold' : 0, 'stl' : 1},
            \ {'short' : 'c', 'long' : 'Classes',            'fold' : 0, 'stl' : 1},
            \ {'short' : 'p', 'long' : 'Properties',         'fold' : 0, 'stl' : 0},
            \ {'short' : 'f', 'long' : 'Fields',             'fold' : 0, 'stl' : 0},
            \ {'short' : 'm', 'long' : 'Methods',            'fold' : 0, 'stl' : 1},
            \ {'short' : 'E', 'long' : 'Error domains',      'fold' : 0, 'stl' : 1},
            \ {'short' : 'r', 'long' : 'Error codes',        'fold' : 0, 'stl' : 1},
            \ {'short' : 'S', 'long' : 'Signals',            'fold' : 0, 'stl' : 1}
        \ ]
        let type_vala.sro = '.'
        " 'enum' doesn't seem to be used as a scope, but it can't hurt to have
        " it here
        let type_vala.kind2scope = {
            \ 's' : 'struct',
            \ 'i' : 'interface',
            \ 'c' : 'class',
            \ 'e' : 'enum'
        \ }
        let type_vala.scope2kind = {
            \ 'struct'    : 's',
            \ 'interface' : 'i',
            \ 'class'     : 'c',
            \ 'enum'      : 'e'
        \ }
        let s:known_types.vala = type_vala
    endif
    if !has_key(s:ctags_types, 'vala') && executable('anjuta-tags')
        let s:known_types.vala.ctagsbin = 'anjuta-tags'
    endif
    " Vera {{{3
    " Why are variables 'virtual'?
    let type_vera = s:TypeInfo.New()
    let type_vera.ctagstype = 'vera'
    let type_vera.kinds     = [
        \ {'short' : 'd', 'long' : 'macros',      'fold' : 1, 'stl' : 0},
        \ {'short' : 'g', 'long' : 'enums',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'T', 'long' : 'typedefs',    'fold' : 0, 'stl' : 0},
        \ {'short' : 'c', 'long' : 'classes',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'enumerators', 'fold' : 0, 'stl' : 0},
        \ {'short' : 'm', 'long' : 'members',     'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',   'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'tasks',       'fold' : 0, 'stl' : 1},
        \ {'short' : 'v', 'long' : 'variables',   'fold' : 0, 'stl' : 0},
        \ {'short' : 'p', 'long' : 'programs',    'fold' : 0, 'stl' : 1}
    \ ]
    let type_vera.sro        = '.' " Nesting doesn't seem to be possible
    let type_vera.kind2scope = {
        \ 'g' : 'enum',
        \ 'c' : 'class',
        \ 'v' : 'virtual'
    \ }
    let type_vera.scope2kind = {
        \ 'enum'      : 'g',
        \ 'class'     : 'c',
        \ 'virtual'   : 'v'
    \ }
    let s:known_types.vera = type_vera
    " Verilog {{{3
    let type_verilog = s:TypeInfo.New()
    let type_verilog.ctagstype = 'verilog'
    let type_verilog.kinds     = [
        \ {'short' : 'c', 'long' : 'constants',           'fold' : 0, 'stl' : 0},
        \ {'short' : 'e', 'long' : 'events',              'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',           'fold' : 0, 'stl' : 1},
        \ {'short' : 'm', 'long' : 'modules',             'fold' : 0, 'stl' : 1},
        \ {'short' : 'n', 'long' : 'net data types',      'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'ports',               'fold' : 0, 'stl' : 1},
        \ {'short' : 'r', 'long' : 'register data types', 'fold' : 0, 'stl' : 1},
        \ {'short' : 't', 'long' : 'tasks',               'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.verilog = type_verilog
    " VHDL {{{3
    " The VHDL ctags parser unfortunately doesn't generate proper scopes
    let type_vhdl = s:TypeInfo.New()
    let type_vhdl.ctagstype = 'vhdl'
    let type_vhdl.kinds     = [
        \ {'short' : 'P', 'long' : 'packages',   'fold' : 1, 'stl' : 0},
        \ {'short' : 'c', 'long' : 'constants',  'fold' : 0, 'stl' : 0},
        \ {'short' : 't', 'long' : 'types',      'fold' : 0, 'stl' : 1},
        \ {'short' : 'T', 'long' : 'subtypes',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'r', 'long' : 'records',    'fold' : 0, 'stl' : 1},
        \ {'short' : 'e', 'long' : 'entities',   'fold' : 0, 'stl' : 1},
        \ {'short' : 'f', 'long' : 'functions',  'fold' : 0, 'stl' : 1},
        \ {'short' : 'p', 'long' : 'procedures', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.vhdl = type_vhdl
    " Vim {{{3
    let type_vim = s:TypeInfo.New()
    let type_vim.ctagstype = 'vim'
    let type_vim.kinds     = [
        \ {'short' : 'v', 'long' : 'variables',          'fold' : 1, 'stl' : 0},
        \ {'short' : 'f', 'long' : 'functions',          'fold' : 0, 'stl' : 1},
        \ {'short' : 'a', 'long' : 'autocommand groups', 'fold' : 1, 'stl' : 1},
        \ {'short' : 'c', 'long' : 'commands',           'fold' : 0, 'stl' : 0},
        \ {'short' : 'm', 'long' : 'maps',               'fold' : 1, 'stl' : 0}
    \ ]
    let s:known_types.vim = type_vim
    " YACC {{{3
    let type_yacc = s:TypeInfo.New()
    let type_yacc.ctagstype = 'yacc'
    let type_yacc.kinds     = [
        \ {'short' : 'l', 'long' : 'labels', 'fold' : 0, 'stl' : 1}
    \ ]
    let s:known_types.yacc = type_yacc
    " }}}3

    call s:LoadUserTypeDefs()

    for type in values(s:known_types)
        call s:CreateTypeKinddict(type)
    endfor

    let s:type_init_done = 1
endfunction

" s:LoadUserTypeDefs() {{{2
function! s:LoadUserTypeDefs(...)
    if a:0 > 0
        let type = a:1

        call s:LogDebugMessage("Initializing user type '" . type . "'")

        let defdict = {}
        let defdict[type] = g:tagbar_type_{type}
    else
        call s:LogDebugMessage('Initializing user types')

        let defdict = tagbar#getusertypes()
    endif

    " Transform the 'kind' definitions into dictionary format
    for def in values(defdict)
        if has_key(def, 'kinds')
            let kinds = def.kinds
            let def.kinds = []
            for kind in kinds
                let kindlist = split(kind, ':')
                let kinddict = {'short' : kindlist[0], 'long' : kindlist[1]}
                if len(kindlist) == 4
                    let kinddict.fold = kindlist[2]
                    let kinddict.stl  = kindlist[3]
                elseif len(kindlist) == 3
                    let kinddict.fold = kindlist[2]
                    let kinddict.stl  = 1
                else
                    let kinddict.fold = 0
                    let kinddict.stl  = 1
                endif
                call add(def.kinds, kinddict)
            endfor
        endif

        " If the user only specified one of kind2scope and scope2kind use it
        " to generate the other one
        if has_key(def, 'kind2scope') && !has_key(def, 'scope2kind')
            let def.scope2kind = {}
            for [key, value] in items(def.kind2scope)
                let def.scope2kind[value] = key
            endfor
        elseif has_key(def, 'scope2kind') && !has_key(def, 'kind2scope')
            let def.kind2scope = {}
            for [key, value] in items(def.scope2kind)
                let def.kind2scope[value] = key
            endfor
        endif
    endfor
    unlet! key value

    for [key, value] in items(defdict)
        if !has_key(s:known_types, key) ||
         \ (has_key(value, 'replace') && value.replace)
            let s:known_types[key] = s:TypeInfo.New(value)
        else
            call extend(s:known_types[key], value)
        endif
    endfor

    if a:0 > 0
        call s:CreateTypeKinddict(s:known_types[type])
    endif
endfunction

" s:CreateTypeKinddict() {{{2
function! s:CreateTypeKinddict(type)
    " Create a dictionary of the kind order for fast access in sorting
    " functions
    let i = 0
    for kind in a:type.kinds
        let a:type.kinddict[kind.short] = i
        let i += 1
    endfor
endfunction

" s:RestoreSession() {{{2
" Properly restore Tagbar after a session got loaded
function! s:RestoreSession()
    call s:LogDebugMessage('Restoring session')

    let curfile = fnamemodify(bufname('%'), ':p')

    let tagbarwinnr = bufwinnr('__Tagbar__')
    if tagbarwinnr == -1
        " Tagbar wasn't open in the saved session, nothing to do
        return
    else
        let in_tagbar = 1
        if winnr() != tagbarwinnr
            call s:winexec(tagbarwinnr . 'wincmd w')
            let in_tagbar = 0
        endif
    endif

    let s:last_autofocus = 0

    call s:Init()

    call s:InitWindow(g:tagbar_autoclose)

    call s:AutoUpdate(curfile)

    if !in_tagbar
        call s:winexec('wincmd p')
    endif
endfunction

" s:MapKeys() {{{2
function! s:MapKeys()
    call s:LogDebugMessage('Mapping keys')

    nnoremap <script> <silent> <buffer> <2-LeftMouse>
                                              \ :call <SID>JumpToTag(0)<CR>
    nnoremap <script> <silent> <buffer> <LeftRelease>
                                 \ <LeftRelease>:call <SID>CheckMouseClick()<CR>

    inoremap <script> <silent> <buffer> <2-LeftMouse>
                                              \ <C-o>:call <SID>JumpToTag(0)<CR>
    inoremap <script> <silent> <buffer> <LeftRelease>
                            \ <LeftRelease><C-o>:call <SID>CheckMouseClick()<CR>

    nnoremap <script> <silent> <buffer> <CR>    :call <SID>JumpToTag(0)<CR>
    nnoremap <script> <silent> <buffer> p       :call <SID>JumpToTag(1)<CR>
    nnoremap <script> <silent> <buffer> <Space> :call <SID>ShowPrototype()<CR>

    nnoremap <script> <silent> <buffer> +        :call <SID>OpenFold()<CR>
    nnoremap <script> <silent> <buffer> <kPlus>  :call <SID>OpenFold()<CR>
    nnoremap <script> <silent> <buffer> zo       :call <SID>OpenFold()<CR>
    nnoremap <script> <silent> <buffer> -        :call <SID>CloseFold()<CR>
    nnoremap <script> <silent> <buffer> <kMinus> :call <SID>CloseFold()<CR>
    nnoremap <script> <silent> <buffer> zc       :call <SID>CloseFold()<CR>
    nnoremap <script> <silent> <buffer> o        :call <SID>ToggleFold()<CR>
    nnoremap <script> <silent> <buffer> za       :call <SID>ToggleFold()<CR>

    nnoremap <script> <silent> <buffer> *    :call <SID>SetFoldLevel(99, 1)<CR>
    nnoremap <script> <silent> <buffer> <kMultiply>
                                           \ :call <SID>SetFoldLevel(99, 1)<CR>
    nnoremap <script> <silent> <buffer> zR   :call <SID>SetFoldLevel(99, 1)<CR>
    nnoremap <script> <silent> <buffer> =    :call <SID>SetFoldLevel(0, 1)<CR>
    nnoremap <script> <silent> <buffer> zM   :call <SID>SetFoldLevel(0, 1)<CR>

    nnoremap <script> <silent> <buffer> <C-N>
                                        \ :call <SID>GotoNextToplevelTag(1)<CR>
    nnoremap <script> <silent> <buffer> <C-P>
                                        \ :call <SID>GotoNextToplevelTag(-1)<CR>

    nnoremap <script> <silent> <buffer> s    :call <SID>ToggleSort()<CR>
    nnoremap <script> <silent> <buffer> x    :call <SID>ZoomWindow()<CR>
    nnoremap <script> <silent> <buffer> q    :call <SID>CloseWindow()<CR>
    nnoremap <script> <silent> <buffer> <F1> :call <SID>ToggleHelp()<CR>
endfunction

" s:CreateAutocommands() {{{2
function! s:CreateAutocommands()
    call s:LogDebugMessage('Creating autocommands')

    augroup TagbarAutoCmds
        autocmd!
        autocmd BufEnter   __Tagbar__ nested call s:QuitIfOnlyWindow()
        autocmd CursorHold __Tagbar__ call s:ShowPrototype()

        autocmd BufWritePost *
            \ if line('$') < g:tagbar_updateonsave_maxlines |
                \ call s:AutoUpdate(fnamemodify(expand('<afile>'), ':p')) |
            \ endif
        autocmd BufEnter,CursorHold,FileType * call
                    \ s:AutoUpdate(fnamemodify(expand('<afile>'), ':p'))
        autocmd BufDelete,BufUnload,BufWipeout * call
                    \ s:known_files.rm(fnamemodify(expand('<afile>'), ':p'))

        autocmd VimEnter * call s:CorrectFocusOnStartup()
    augroup END

    let s:autocommands_done = 1
endfunction

" s:CheckForExCtags() {{{2
" Test whether the ctags binary is actually Exuberant Ctags and not GNU ctags
" (or something else)
function! s:CheckForExCtags()
    call s:LogDebugMessage('Checking for Exuberant Ctags')

    let ctags_cmd = s:EscapeCtagsCmd(g:tagbar_ctags_bin, '--version')
    if ctags_cmd == ''
        return
    endif

    let ctags_output = s:ExecuteCtags(ctags_cmd)

    if v:shell_error || ctags_output !~# 'Exuberant Ctags'
        echoerr 'Tagbar: Ctags doesn''t seem to be Exuberant Ctags!'
        echomsg 'GNU ctags will NOT WORK.'
              \ 'Please download Exuberant Ctags from ctags.sourceforge.net'
              \ 'and install it in a directory in your $PATH'
              \ 'or set g:tagbar_ctags_bin.'
        echomsg 'Executed command: "' . ctags_cmd . '"'
        if !empty(ctags_output)
            echomsg 'Command output:'
            for line in split(ctags_output, '\n')
                echomsg line
            endfor
        endif
        return 0
    elseif !s:CheckExCtagsVersion(ctags_output)
        echoerr 'Tagbar: Exuberant Ctags is too old!'
        echomsg 'You need at least version 5.5 for Tagbar to work.'
              \ 'Please download a newer version from ctags.sourceforge.net.'
        echomsg 'Executed command: "' . ctags_cmd . '"'
        if !empty(ctags_output)
            echomsg 'Command output:'
            for line in split(ctags_output, '\n')
                echomsg line
            endfor
        endif
        return 0
    else
        let s:checked_ctags = 1
        return 1
    endif
endfunction

" s:CheckExCtagsVersion() {{{2
function! s:CheckExCtagsVersion(output)
    call s:LogDebugMessage('Checking Exuberant Ctags version')

    if a:output =~ 'Exuberant Ctags Development'
        return 1
    endif

    let matchlist = matchlist(a:output, '\vExuberant Ctags (\d+)\.(\d+)')
    let major     = matchlist[1]
    let minor     = matchlist[2]

    return major >= 6 || (major == 5 && minor >= 5)
endfunction

" s:CheckFTCtags() {{{2
function! s:CheckFTCtags(bin, ftype)
    if executable(a:bin)
        return a:bin
    endif

    if exists('g:tagbar_type_' . a:ftype)
        execute 'let userdef = ' . 'g:tagbar_type_' . a:ftype
        if has_key(userdef, 'ctagsbin')
            return userdef.ctagsbin
        else
            return ''
        endif
    endif

    return ''
endfunction

" s:GetSupportedFiletypes() {{{2
function! s:GetSupportedFiletypes()
    call s:LogDebugMessage('Getting filetypes sypported by Exuberant Ctags')

    let ctags_cmd = s:EscapeCtagsCmd(g:tagbar_ctags_bin, '--list-languages')
    if ctags_cmd == ''
        return
    endif

    let ctags_output = s:ExecuteCtags(ctags_cmd)

    if v:shell_error
        " this shouldn't happen as potential problems would have already been
        " caught by the previous ctags checking
        return
    endif

    let types = split(ctags_output, '\n\+')

    for type in types
        let s:ctags_types[tolower(type)] = 1
    endfor

    let s:checked_ctags_types = 1
endfunction

" Prototypes {{{1
" Base tag {{{2
let s:BaseTag = {}

" s:BaseTag.New() {{{3
function! s:BaseTag.New(name) dict
    let newobj = copy(self)

    call newobj._init(a:name)

    return newobj
endfunction

" s:BaseTag._init() {{{3
function! s:BaseTag._init(name) dict
    let self.name          = a:name
    let self.fields        = {}
    let self.fields.line   = 0
    let self.fields.column = 1
    let self.path          = ''
    let self.fullpath      = a:name
    let self.depth         = 0
    let self.parent        = {}
    let self.tline         = -1
    let self.fileinfo      = {}
    let self.typeinfo      = {}
endfunction

" s:BaseTag.isNormalTag() {{{3
function! s:BaseTag.isNormalTag() dict
    return 0
endfunction

" s:BaseTag.isPseudoTag() {{{3
function! s:BaseTag.isPseudoTag() dict
    return 0
endfunction

" s:BaseTag.isKindheader() {{{3
function! s:BaseTag.isKindheader() dict
    return 0
endfunction

" s:BaseTag.getPrototype() {{{3
function! s:BaseTag.getPrototype() dict
    return ''
endfunction

" s:BaseTag._getPrefix() {{{3
function! s:BaseTag._getPrefix() dict
    let fileinfo = self.fileinfo

    if has_key(self, 'children') && !empty(self.children)
        if fileinfo.tagfolds[self.fields.kind][self.fullpath]
            let prefix = s:icon_closed
        else
            let prefix = s:icon_open
        endif
    else
        let prefix = ' '
    endif
    if has_key(self.fields, 'access')
        let prefix .= get(s:access_symbols, self.fields.access, ' ')
    else
        let prefix .= ' '
    endif

    return prefix
endfunction

" s:BaseTag.initFoldState() {{{3
function! s:BaseTag.initFoldState() dict
    let fileinfo = self.fileinfo

    if s:known_files.has(fileinfo.fpath) &&
     \ has_key(fileinfo._tagfolds_old[self.fields.kind], self.fullpath)
        " The file has been updated and the tag was there before, so copy its
        " old fold state
        let fileinfo.tagfolds[self.fields.kind][self.fullpath] =
                    \ fileinfo._tagfolds_old[self.fields.kind][self.fullpath]
    elseif self.depth >= fileinfo.foldlevel
        let fileinfo.tagfolds[self.fields.kind][self.fullpath] = 1
    else
        let fileinfo.tagfolds[self.fields.kind][self.fullpath] =
                    \ fileinfo.kindfolds[self.fields.kind]
    endif
endfunction

" s:BaseTag.getClosedParentTline() {{{3
function! s:BaseTag.getClosedParentTline() dict
    let tagline = self.tline
    let fileinfo = self.fileinfo

    let parent = self.parent
    while !empty(parent)
        if parent.isFolded()
            let tagline = parent.tline
            break
        endif
        let parent = parent.parent
    endwhile

    return tagline
endfunction

" s:BaseTag.isFoldable() {{{3
function! s:BaseTag.isFoldable() dict
    return has_key(self, 'children') && !empty(self.children)
endfunction

" s:BaseTag.isFolded() {{{3
function! s:BaseTag.isFolded() dict
    return self.fileinfo.tagfolds[self.fields.kind][self.fullpath]
endfunction

" s:BaseTag.openFold() {{{3
function! s:BaseTag.openFold() dict
    if self.isFoldable()
        let self.fileinfo.tagfolds[self.fields.kind][self.fullpath] = 0
    endif
endfunction

" s:BaseTag.closeFold() {{{3
function! s:BaseTag.closeFold() dict
    let newline = line('.')

    if !empty(self.parent) && self.parent.isKindheader()
        " Tag is child of generic 'kind'
        call self.parent.closeFold()
        let newline = self.parent.tline
    elseif self.isFoldable() && !self.isFolded()
        " Tag is parent of a scope and is not folded
        let self.fileinfo.tagfolds[self.fields.kind][self.fullpath] = 1
        let newline = self.tline
    elseif !empty(self.parent)
        " Tag is normal child, so close parent
        let parent = self.parent
        let self.fileinfo.tagfolds[parent.fields.kind][parent.fullpath] = 1
        let newline = parent.tline
    endif

    return newline
endfunction

" s:BaseTag.setFolded() {{{3
function! s:BaseTag.setFolded(folded) dict
    let self.fileinfo.tagfolds[self.fields.kind][self.fullpath] = a:folded
endfunction

" s:BaseTag.openParents() {{{3
function! s:BaseTag.openParents() dict
    let parent = self.parent

    while !empty(parent)
        call parent.openFold()
        let parent = parent.parent
    endwhile
endfunction

" Normal tag {{{2
let s:NormalTag = copy(s:BaseTag)

" s:NormalTag.isNormalTag() {{{3
function! s:NormalTag.isNormalTag() dict
    return 1
endfunction

" s:NormalTag.strfmt() {{{3
function! s:NormalTag.strfmt() dict
    let fileinfo = self.fileinfo
    let typeinfo = self.typeinfo

    let suffix = get(self.fields, 'signature', '')
    if has_key(self.fields, 'type')
        let suffix .= ' : ' . self.fields.type
    elseif has_key(typeinfo, 'kind2scope') &&
         \ has_key(typeinfo.kind2scope, self.fields.kind)
        let suffix .= ' : ' . typeinfo.kind2scope[self.fields.kind]
    endif

    return self._getPrefix() . self.name . suffix . "\n"
endfunction

" s:NormalTag.str() {{{3
function! s:NormalTag.str(longsig, full) dict
    if a:full && self.path != ''
        let str = self.path . self.typeinfo.sro . self.name
    else
        let str = self.name
    endif

    if has_key(self.fields, 'signature')
        if a:longsig
            let str .= self.fields.signature
        else
            let str .= '()'
        endif
    endif

    return str
endfunction

" s:NormalTag.getPrototype() {{{3
function! s:NormalTag.getPrototype() dict
    return self.prototype
endfunction

" Pseudo tag {{{2
let s:PseudoTag = copy(s:BaseTag)

" s:PseudoTag.isPseudoTag() {{{3
function! s:PseudoTag.isPseudoTag() dict
    return 1
endfunction

" s:PseudoTag.strfmt() {{{3
function! s:PseudoTag.strfmt() dict
    let fileinfo = self.fileinfo
    let typeinfo = self.typeinfo

    let suffix = get(self.fields, 'signature', '')
    if has_key(typeinfo.kind2scope, self.fields.kind)
        let suffix .= ' : ' . typeinfo.kind2scope[self.fields.kind]
    endif

    return self._getPrefix() . self.name . '*' . suffix
endfunction

" Kind header {{{2
let s:KindheaderTag = copy(s:BaseTag)

" s:KindheaderTag.isKindheader() {{{3
function! s:KindheaderTag.isKindheader() dict
    return 1
endfunction

" s:KindheaderTag.getPrototype() {{{3
function! s:KindheaderTag.getPrototype() dict
    return self.name . ': ' .
         \ self.numtags . ' ' . (self.numtags > 1 ? 'tags' : 'tag')
endfunction

" s:KindheaderTag.isFoldable() {{{3
function! s:KindheaderTag.isFoldable() dict
    return 1
endfunction

" s:KindheaderTag.isFolded() {{{3
function! s:KindheaderTag.isFolded() dict
    return self.fileinfo.kindfolds[self.short]
endfunction

" s:KindheaderTag.openFold() {{{3
function! s:KindheaderTag.openFold() dict
    let self.fileinfo.kindfolds[self.short] = 0
endfunction

" s:KindheaderTag.closeFold() {{{3
function! s:KindheaderTag.closeFold() dict
    let self.fileinfo.kindfolds[self.short] = 1
    return line('.')
endfunction

" s:KindheaderTag.toggleFold() {{{3
function! s:KindheaderTag.toggleFold() dict
    let fileinfo = s:known_files.getCurrent()

    let fileinfo.kindfolds[self.short] = !fileinfo.kindfolds[self.short]
endfunction

" Type info {{{2
let s:TypeInfo = {}

" s:TypeInfo.New() {{{3
function! s:TypeInfo.New(...) dict
    let newobj = copy(self)

    let newobj.kinddict = {}

    if a:0 > 0
        call extend(newobj, a:1)
    endif

    return newobj
endfunction

" s:TypeInfo.getKind() {{{3
function! s:TypeInfo.getKind(kind) dict
    let idx = self.kinddict[a:kind]
    return self.kinds[idx]
endfunction

" File info {{{2
let s:FileInfo = {}

" s:FileInfo.New() {{{3
function! s:FileInfo.New(fname, ftype) dict
    let newobj = copy(self)

    " The complete file path
    let newobj.fpath = a:fname

    " File modification time
    let newobj.mtime = getftime(a:fname)

    " The vim file type
    let newobj.ftype = a:ftype

    " List of the tags that are present in the file, sorted according to the
    " value of 'g:tagbar_sort'
    let newobj.tags = []

    " Dictionary of the tags, indexed by line number in the file
    let newobj.fline = {}

    " Dictionary of the tags, indexed by line number in the tagbar
    let newobj.tline = {}

    " Dictionary of the folding state of 'kind's, indexed by short name
    let newobj.kindfolds = {}
    let typeinfo = s:known_types[a:ftype]
    let newobj.typeinfo = typeinfo
    " copy the default fold state from the type info
    for kind in typeinfo.kinds
        let newobj.kindfolds[kind.short] =
                    \ g:tagbar_foldlevel == 0 ? 1 : kind.fold
    endfor

    " Dictionary of dictionaries of the folding state of individual tags,
    " indexed by kind and full path
    let newobj.tagfolds = {}
    for kind in typeinfo.kinds
        let newobj.tagfolds[kind.short] = {}
    endfor

    " The current foldlevel of the file
    let newobj.foldlevel = g:tagbar_foldlevel

    return newobj
endfunction

" s:FileInfo.reset() {{{3
" Reset stuff that gets regenerated while processing a file and save the old
" tag folds
function! s:FileInfo.reset() dict
    let self.mtime = getftime(self.fpath)
    let self.tags  = []
    let self.fline = {}
    let self.tline = {}

    let self._tagfolds_old = self.tagfolds
    let self.tagfolds = {}

    let typeinfo = s:known_types[self.ftype]
    for kind in typeinfo.kinds
        let self.tagfolds[kind.short] = {}
    endfor
endfunction

" s:FileInfo.clearOldFolds() {{{3
function! s:FileInfo.clearOldFolds() dict
    if exists('self._tagfolds_old')
        unlet self._tagfolds_old
    endif
endfunction

" s:FileInfo.sortTags() {{{3
function! s:FileInfo.sortTags() dict
    if has_key(s:compare_typeinfo, 'sort')
        if s:compare_typeinfo.sort
            call s:SortTags(self.tags, 's:CompareByKind')
        else
            call s:SortTags(self.tags, 's:CompareByLine')
        endif
    elseif g:tagbar_sort
        call s:SortTags(self.tags, 's:CompareByKind')
    else
        call s:SortTags(self.tags, 's:CompareByLine')
    endif
endfunction

" s:FileInfo.openKindFold() {{{3
function! s:FileInfo.openKindFold(kind) dict
    let self.kindfolds[a:kind.short] = 0
endfunction

" s:FileInfo.closeKindFold() {{{3
function! s:FileInfo.closeKindFold(kind) dict
    let self.kindfolds[a:kind.short] = 1
endfunction

" Known files {{{2
let s:known_files = {
    \ '_current' : {},
    \ '_files'   : {}
\ }

" s:known_files.getCurrent() {{{3
function! s:known_files.getCurrent() dict
    return self._current
endfunction

" s:known_files.setCurrent() {{{3
function! s:known_files.setCurrent(fileinfo) dict
    let self._current = a:fileinfo
endfunction

" s:known_files.get() {{{3
function! s:known_files.get(fname) dict
    return get(self._files, a:fname, {})
endfunction

" s:known_files.put() {{{3
" Optional second argument is the filename
function! s:known_files.put(fileinfo, ...) dict
    if a:0 == 1
        let self._files[a:1] = a:fileinfo
    else
        let fname = a:fileinfo.fpath
        let self._files[fname] = a:fileinfo
    endif
endfunction

" s:known_files.has() {{{3
function! s:known_files.has(fname) dict
    return has_key(self._files, a:fname)
endfunction

" s:known_files.rm() {{{3
function! s:known_files.rm(fname) dict
    if s:known_files.has(a:fname)
        call remove(self._files, a:fname)
    endif
endfunction

" Window management {{{1
" s:ToggleWindow() {{{2
function! s:ToggleWindow()
    call s:LogDebugMessage('ToggleWindow called')

    let tagbarwinnr = bufwinnr("__Tagbar__")
    if tagbarwinnr != -1
        call s:CloseWindow()
        return
    endif

    call s:OpenWindow('')

    call s:LogDebugMessage('ToggleWindow finished')
endfunction

" s:OpenWindow() {{{2
function! s:OpenWindow(flags)
    call s:LogDebugMessage("OpenWindow called with flags: '" . a:flags . "'")

    let autofocus = a:flags =~# 'f'
    let jump      = a:flags =~# 'j'
    let autoclose = a:flags =~# 'c'

    let curfile = fnamemodify(bufname('%'), ':p')
    let curline = line('.')

    " If the tagbar window is already open check jump flag
    " Also set the autoclose flag if requested
    let tagbarwinnr = bufwinnr('__Tagbar__')
    if tagbarwinnr != -1
        if winnr() != tagbarwinnr && jump
            call s:winexec(tagbarwinnr . 'wincmd w')
            if autoclose
                let w:autoclose = autoclose
            endif
            call s:HighlightTag(1, curline)
        endif
        call s:LogDebugMessage("OpenWindow finished, Tagbar already open")
        return
    endif

    " This is only needed for the CorrectFocusOnStartup() function
    let s:last_autofocus = autofocus

    if !s:Init()
        return 0
    endif

    " Expand the Vim window to accomodate for the Tagbar window if requested
    if g:tagbar_expand && !s:window_expanded && has('gui_running')
        let &columns += g:tagbar_width + 1
        let s:window_expanded = 1
    endif

    let eventignore_save = &eventignore
    set eventignore=all

    let openpos = g:tagbar_left ? 'topleft vertical ' : 'botright vertical '
    exe 'silent keepalt ' . openpos . g:tagbar_width . 'split ' . '__Tagbar__'

    let &eventignore = eventignore_save

    call s:InitWindow(autoclose)

    call s:AutoUpdate(curfile)
    call s:HighlightTag(1, curline)

    if !(g:tagbar_autoclose || autofocus || g:tagbar_autofocus)
        call s:winexec('wincmd p')
    endif

    call s:LogDebugMessage('OpenWindow finished')
endfunction

" s:InitWindow() {{{2
function! s:InitWindow(autoclose)
    call s:LogDebugMessage('InitWindow called with autoclose: ' . a:autoclose)

    setlocal noreadonly " in case the "view" mode is used
    setlocal buftype=nofile
    setlocal bufhidden=hide
    setlocal noswapfile
    setlocal nobuflisted
    setlocal nomodifiable
    setlocal filetype=tagbar
    setlocal nolist
    setlocal nonumber
    setlocal nowrap
    setlocal winfixwidth
    setlocal textwidth=0
    setlocal nocursorline
    setlocal nocursorcolumn

    if exists('+relativenumber')
        setlocal norelativenumber
    endif

    setlocal nofoldenable
    setlocal foldcolumn=0
    " Reset fold settings in case a plugin set them globally to something
    " expensive. Apparently 'foldexpr' gets executed even if 'foldenable' is
    " off, and then for every appended line (like with :put).
    setlocal foldmethod&
    setlocal foldexpr&

    " Earlier versions have a bug in local, evaluated statuslines
    if v:version > 701 || (v:version == 701 && has('patch097'))
        setlocal statusline=%!TagbarGenerateStatusline()
    else
        setlocal statusline=Tagbar
    endif

    " Script-local variable needed since compare functions can't
    " take extra arguments
    let s:compare_typeinfo = {}

    let s:is_maximized = 0
    let s:short_help   = 1
    let s:new_window   = 1

    let w:autoclose = a:autoclose

    if has('balloon_eval')
        setlocal balloonexpr=TagbarBalloonExpr()
        set ballooneval
    endif

    let cpoptions_save = &cpoptions
    set cpoptions&vim

    if !hasmapto('JumpToTag', 'n')
        call s:MapKeys()
    endif

    let &cpoptions = cpoptions_save

    call s:LogDebugMessage('InitWindow finished')
endfunction

" s:CloseWindow() {{{2
function! s:CloseWindow()
    call s:LogDebugMessage('CloseWindow called')

    let tagbarwinnr = bufwinnr('__Tagbar__')
    if tagbarwinnr == -1
        return
    endif

    let tagbarbufnr = winbufnr(tagbarwinnr)

    if winnr() == tagbarwinnr
        if winbufnr(2) != -1
            " Other windows are open, only close the tagbar one

            let curfile = s:known_files.getCurrent()

            call s:winexec('close')

            " Try to jump to the correct window after closing
            call s:winexec('wincmd p')

            if !empty(curfile)
                let filebufnr = bufnr(curfile.fpath)

                if bufnr('%') != filebufnr
                    let filewinnr = bufwinnr(filebufnr)
                    if filewinnr != -1
                        call s:winexec(filewinnr . 'wincmd w')
                    endif
                endif
            endif
        endif
    else
        " Go to the tagbar window, close it and then come back to the
        " original window
        let curbufnr = bufnr('%')
        call s:winexec(tagbarwinnr . 'wincmd w')
        close
        " Need to jump back to the original window only if we are not
        " already in that window
        let winnum = bufwinnr(curbufnr)
        if winnr() != winnum
            call s:winexec(winnum . 'wincmd w')
        endif
    endif

    " If the Vim window has been expanded, and Tagbar is not open in any other
    " tabpages, shrink the window again
    if s:window_expanded
        let tablist = []
        for i in range(tabpagenr('$'))
            call extend(tablist, tabpagebuflist(i + 1))
        endfor

        if index(tablist, tagbarbufnr) == -1
            let &columns -= g:tagbar_width + 1
            let s:window_expanded = 0
        endif
    endif

    call s:LogDebugMessage('CloseWindow finished')
endfunction

" s:ZoomWindow() {{{2
function! s:ZoomWindow()
    if s:is_maximized
        execute 'vert resize ' . g:tagbar_width
        let s:is_maximized = 0
    else
        vert resize
        let s:is_maximized = 1
    endif
endfunction

" s:CorrectFocusOnStartup() {{{2
" For whatever reason the focus will be on the Tagbar window if
" tagbar#autoopen is used with a FileType autocommand on startup and
" g:tagbar_left is set. This should work around it by jumping to the window of
" the current file after startup.
function! s:CorrectFocusOnStartup()
    if bufwinnr('__Tagbar__') != -1 && !g:tagbar_autofocus && !s:last_autofocus
        let curfile = s:known_files.getCurrent()
        if !empty(curfile) && curfile.fpath != fnamemodify(bufname('%'), ':p')
            let winnr = bufwinnr(curfile.fpath)
            if winnr != -1
                call s:winexec(winnr . 'wincmd w')
            endif
        endif
    endif
endfunction

" Tag processing {{{1
" s:ProcessFile() {{{2
" Execute ctags and put the information into a 'FileInfo' object
function! s:ProcessFile(fname, ftype)
    call s:LogDebugMessage('ProcessFile called on ' . a:fname)

    if !s:IsValidFile(a:fname, a:ftype)
        call s:LogDebugMessage('Not a valid file, returning')
        return
    endif

    let ctags_output = s:ExecuteCtagsOnFile(a:fname, a:ftype)

    if ctags_output == -1
        call s:LogDebugMessage('Ctags error when processing file')
        " put an empty entry into known_files so the error message is only
        " shown once
        call s:known_files.put({}, a:fname)
        return
    elseif ctags_output == ''
        call s:LogDebugMessage('Ctags output empty')
        " No need to go through the tag processing if there are no tags, and
        " preserving the old fold state also isn't necessary
        call s:known_files.put(s:FileInfo.New(a:fname, a:ftype), a:fname)
        return
    endif

    " If the file has only been updated preserve the fold states, otherwise
    " create a new entry
    if s:known_files.has(a:fname)
        let fileinfo = s:known_files.get(a:fname)
        call fileinfo.reset()
    else
        let fileinfo = s:FileInfo.New(a:fname, a:ftype)
    endif

    let typeinfo = fileinfo.typeinfo

    " Parse the ctags output lines
    call s:LogDebugMessage('Parsing ctags output')
    let rawtaglist = split(ctags_output, '\n\+')
    for line in rawtaglist
        " skip comments
        if line =~# '^!_TAG_'
            continue
        endif

        let parts = split(line, ';"')
        if len(parts) == 2 " Is a valid tag line
            let taginfo = s:ParseTagline(parts[0], parts[1], typeinfo, fileinfo)
            let fileinfo.fline[taginfo.fields.line] = taginfo
            call add(fileinfo.tags, taginfo)
        endif
    endfor

    " Process scoped tags
    let processedtags = []
    if has_key(typeinfo, 'kind2scope')
        call s:LogDebugMessage('Processing scoped tags')

        let scopedtags = []
        let is_scoped = 'has_key(typeinfo.kind2scope, v:val.fields.kind) ||
                       \ has_key(v:val, "scope")'
        let scopedtags += filter(copy(fileinfo.tags), is_scoped)
        call filter(fileinfo.tags, '!(' . is_scoped . ')')

        call s:AddScopedTags(scopedtags, processedtags, {}, 0,
                           \ typeinfo, fileinfo)

        if !empty(scopedtags)
            echoerr 'Tagbar: ''scopedtags'' not empty after processing,'
                  \ 'this should never happen!'
                  \ 'Please contact the script maintainer with an example.'
        endif
    endif
    call s:LogDebugMessage('Number of top-level tags: ' . len(processedtags))

    " Create a placeholder tag for the 'kind' header for folding purposes
    for kind in typeinfo.kinds

        let curtags = filter(copy(fileinfo.tags),
                           \ 'v:val.fields.kind ==# kind.short')
        call s:LogDebugMessage('Processing kind: ' . kind.short .
                             \ ', number of tags: ' . len(curtags))

        if empty(curtags)
            continue
        endif

        let kindtag          = s:KindheaderTag.New(kind.long)
        let kindtag.short    = kind.short
        let kindtag.numtags  = len(curtags)
        let kindtag.fileinfo = fileinfo

        for tag in curtags
            let tag.parent = kindtag
        endfor
    endfor

    if !empty(processedtags)
        call extend(fileinfo.tags, processedtags)
    endif

    " Clear old folding information from previous file version to prevent leaks
    call fileinfo.clearOldFolds()

    " Sort the tags
    let s:compare_typeinfo = typeinfo
    call fileinfo.sortTags()

    call s:known_files.put(fileinfo)
endfunction

" s:ExecuteCtagsOnFile() {{{2
function! s:ExecuteCtagsOnFile(fname, ftype)
    call s:LogDebugMessage('ExecuteCtagsOnFile called on ' . a:fname)

    let typeinfo = s:known_types[a:ftype]

    if has_key(typeinfo, 'ctagsargs')
        let ctags_args = ' ' . typeinfo.ctagsargs . ' '
    else
        let ctags_args  = ' -f - '
        let ctags_args .= ' --format=2 '
        let ctags_args .= ' --excmd=pattern '
        let ctags_args .= ' --fields=nksSa '
        let ctags_args .= ' --extra= '
        let ctags_args .= ' --sort=yes '

        " Include extra type definitions
        if has_key(typeinfo, 'deffile')
            let ctags_args .= ' --options=' . typeinfo.deffile . ' '
        endif

        let ctags_type = typeinfo.ctagstype

        let ctags_kinds = ''
        for kind in typeinfo.kinds
            let ctags_kinds .= kind.short
        endfor

        let ctags_args .= ' --language-force=' . ctags_type .
                        \ ' --' . ctags_type . '-kinds=' . ctags_kinds . ' '
    endif

    if has_key(typeinfo, 'ctagsbin')
        " reset 'wildignore' temporarily in case *.exe is included in it
        let wildignore_save = &wildignore
        set wildignore&
        let ctags_bin = expand(typeinfo.ctagsbin)
        let &wildignore = wildignore_save
    else
        let ctags_bin = g:tagbar_ctags_bin
    endif

    let ctags_cmd = s:EscapeCtagsCmd(ctags_bin, ctags_args, a:fname)
    if ctags_cmd == ''
        return ''
    endif

    let ctags_output = s:ExecuteCtags(ctags_cmd)

    if v:shell_error || ctags_output =~ 'Warning: cannot open source file'
        echoerr 'Tagbar: Could not execute ctags for ' . a:fname . '!'
        echomsg 'Executed command: "' . ctags_cmd . '"'
        if !empty(ctags_output)
            call s:LogDebugMessage('Command output:')
            call s:LogDebugMessage(ctags_output)
            echomsg 'Command output:'
            for line in split(ctags_output, '\n')
                echomsg line
            endfor
        endif
        return -1
    endif

    call s:LogDebugMessage('Ctags executed successfully')
    return ctags_output
endfunction

" s:ParseTagline() {{{2
" Structure of a tag line:
" tagname<TAB>filename<TAB>expattern;"fields
" fields: <TAB>name:value
" fields that are always present: kind, line
function! s:ParseTagline(part1, part2, typeinfo, fileinfo)
    let basic_info  = split(a:part1, '\t')

    let taginfo      = s:NormalTag.New(basic_info[0])
    let taginfo.file = basic_info[1]

    " the pattern can contain tabs and thus may have been split up, so join
    " the rest of the items together again
    let pattern = join(basic_info[2:], "\t")
    let start   = 2 " skip the slash and the ^
    let end     = strlen(pattern) - 1
    if pattern[end - 1] ==# '$'
        let end -= 1
        let dollar = '\$'
    else
        let dollar = ''
    endif
    let pattern           = strpart(pattern, start, end - start)
    let taginfo.pattern   = '\V\^\C' . pattern . dollar
    let prototype         = substitute(pattern,   '^[[:space:]]\+', '', '')
    let prototype         = substitute(prototype, '[[:space:]]\+$', '', '')
    let taginfo.prototype = prototype

    let fields = split(a:part2, '\t')
    let taginfo.fields.kind = remove(fields, 0)
    for field in fields
        " can't use split() since the value can contain ':'
        let delimit = stridx(field, ':')
        let key     = strpart(field, 0, delimit)
        let val     = strpart(field, delimit + 1)
        if len(val) > 0
            let taginfo.fields[key] = val
        endif
    endfor
    " Needed for jsctags
    if has_key(taginfo.fields, 'lineno')
        let taginfo.fields.line = taginfo.fields.lineno
    endif

    " Make some information easier accessible
    if has_key(a:typeinfo, 'scope2kind')
        for scope in keys(a:typeinfo.scope2kind)
            if has_key(taginfo.fields, scope)
                let taginfo.scope = scope
                let taginfo.path  = taginfo.fields[scope]

                let taginfo.fullpath = taginfo.path . a:typeinfo.sro .
                                     \ taginfo.name
                break
            endif
        endfor
        let taginfo.depth = len(split(taginfo.path, '\V' . a:typeinfo.sro))
    endif

    let taginfo.fileinfo = a:fileinfo
    let taginfo.typeinfo = a:typeinfo

    " Needed for folding
    try
        call taginfo.initFoldState()
    catch /^Vim(\a\+):E716:/ " 'Key not present in Dictionary'
        " The tag has a 'kind' that doesn't exist in the type definition
        echoerr 'Your ctags and Tagbar configurations are out of sync!'
              \ 'Please read '':help tagbar-extend''.'
    endtry

    return taginfo
endfunction

" s:AddScopedTags() {{{2
" Recursively process tags. Unfortunately there is a problem: not all tags in
" a hierarchy are actually there. For example, in C++ a class can be defined
" in a header file and implemented in a .cpp file (so the class itself doesn't
" appear in the .cpp file and thus doesn't generate a tag). Another example
" are anonymous structures like namespaces, structs, enums, and unions, that
" also don't get a tag themselves. These tags are thus called 'pseudo-tags' in
" Tagbar. Properly parsing them is quite tricky, so try not to think about it
" too much.
function! s:AddScopedTags(tags, processedtags, parent, depth,
                        \ typeinfo, fileinfo)
    if !empty(a:parent)
        let curpath = a:parent.fullpath
        let pscope  = a:typeinfo.kind2scope[a:parent.fields.kind]
    else
        let curpath = ''
        let pscope  = ''
    endif

    let is_cur_tag = 'v:val.depth == a:depth'

    if !empty(curpath)
        " Check whether the tag is either a direct child at the current depth
        " or at least a proper grandchild with pseudo-tags in between. If it
        " is a direct child also check for matching scope.
        let is_cur_tag .= ' &&
        \ (v:val.path ==# curpath ||
         \ match(v:val.path, ''\V\^\C'' . curpath . a:typeinfo.sro) == 0) &&
        \ (v:val.path ==# curpath ? (v:val.scope ==# pscope) : 1)'
    endif

    let curtags = filter(copy(a:tags), is_cur_tag)

    if !empty(curtags)
        call filter(a:tags, '!(' . is_cur_tag . ')')

        let realtags   = []
        let pseudotags = []

        while !empty(curtags)
            let tag = remove(curtags, 0)

            if tag.path != curpath
                " tag is child of a pseudo-tag, so create a new pseudo-tag and
                " add all its children to it
                let pseudotag = s:ProcessPseudoTag(curtags, tag, a:parent,
                                                 \ a:typeinfo, a:fileinfo)

                call add(pseudotags, pseudotag)
            else
                call add(realtags, tag)
            endif
        endwhile

        " Recursively add the children of the tags on the current level
        for tag in realtags
            let tag.parent = a:parent

            if !has_key(a:typeinfo.kind2scope, tag.fields.kind)
                continue
            endif

            if !has_key(tag, 'children')
                let tag.children = []
            endif

            call s:AddScopedTags(a:tags, tag.children, tag, a:depth + 1,
                               \ a:typeinfo, a:fileinfo)
        endfor
        call extend(a:processedtags, realtags)

        " Recursively add the children of the tags that are children of the
        " pseudo-tags on the current level
        for tag in pseudotags
            call s:ProcessPseudoChildren(a:tags, tag, a:depth, a:typeinfo,
                                       \ a:fileinfo)
        endfor
        call extend(a:processedtags, pseudotags)
    endif

    " Now we have to check if there are any pseudo-tags at the current level
    " so we have to check for real tags at a lower level, i.e. grandchildren
    let is_grandchild = 'v:val.depth > a:depth'

    if !empty(curpath)
        let is_grandchild .=
        \ ' && match(v:val.path, ''\V\^\C'' . curpath . a:typeinfo.sro) == 0'
    endif

    let grandchildren = filter(copy(a:tags), is_grandchild)

    if !empty(grandchildren)
        call s:AddScopedTags(a:tags, a:processedtags, a:parent, a:depth + 1,
                           \ a:typeinfo, a:fileinfo)
    endif
endfunction

" s:ProcessPseudoTag() {{{2
function! s:ProcessPseudoTag(curtags, tag, parent, typeinfo, fileinfo)
    let curpath = !empty(a:parent) ? a:parent.fullpath : ''

    let pseudoname = substitute(a:tag.path, curpath, '', '')
    let pseudoname = substitute(pseudoname, '\V\^' . a:typeinfo.sro, '', '')
    let pseudotag  = s:CreatePseudoTag(pseudoname, a:parent, a:tag.scope,
                                     \ a:typeinfo, a:fileinfo)
    let pseudotag.children = [a:tag]

    " get all the other (direct) children of the current pseudo-tag
    let ispseudochild = 'v:val.path ==# a:tag.path && v:val.scope ==# a:tag.scope'
    let pseudochildren = filter(copy(a:curtags), ispseudochild)
    if !empty(pseudochildren)
        call filter(a:curtags, '!(' . ispseudochild . ')')
        call extend(pseudotag.children, pseudochildren)
    endif

    return pseudotag
endfunction

" s:ProcessPseudoChildren() {{{2
function! s:ProcessPseudoChildren(tags, tag, depth, typeinfo, fileinfo)
    for childtag in a:tag.children
        let childtag.parent = a:tag

        if !has_key(a:typeinfo.kind2scope, childtag.fields.kind)
            continue
        endif

        if !has_key(childtag, 'children')
            let childtag.children = []
        endif

        call s:AddScopedTags(a:tags, childtag.children, childtag, a:depth + 1,
                           \ a:typeinfo, a:fileinfo)
    endfor

    let is_grandchild = 'v:val.depth > a:depth &&
                       \ match(v:val.path, ''^\C'' . a:tag.fullpath) == 0'
    let grandchildren = filter(copy(a:tags), is_grandchild)
    if !empty(grandchildren)
        call s:AddScopedTags(a:tags, a:tag.children, a:tag, a:depth + 1,
                           \ a:typeinfo, a:fileinfo)
    endif
endfunction

" s:CreatePseudoTag() {{{2
function! s:CreatePseudoTag(name, parent, scope, typeinfo, fileinfo)
    if !empty(a:parent)
        let curpath = a:parent.fullpath
        let pscope  = a:typeinfo.kind2scope[a:parent.fields.kind]
    else
        let curpath = ''
        let pscope  = ''
    endif

    let pseudotag             = s:PseudoTag.New(a:name)
    let pseudotag.fields.kind = a:typeinfo.scope2kind[a:scope]

    let parentscope = substitute(curpath, a:name . '$', '', '')
    let parentscope = substitute(parentscope,
                               \ '\V\^' . a:typeinfo.sro . '\$', '', '')

    if pscope != ''
        let pseudotag.fields[pscope] = parentscope
        let pseudotag.scope    = pscope
        let pseudotag.path     = parentscope
        let pseudotag.fullpath =
                    \ pseudotag.path . a:typeinfo.sro . pseudotag.name
    endif
    let pseudotag.depth = len(split(pseudotag.path, '\V' . a:typeinfo.sro))

    let pseudotag.parent = a:parent

    let pseudotag.fileinfo = a:fileinfo
    let pseudotag.typeinfo = a:typeinfo

    call pseudotag.initFoldState()

    return pseudotag
endfunction

" Sorting {{{1
" s:SortTags() {{{2
function! s:SortTags(tags, comparemethod)
    call sort(a:tags, a:comparemethod)

    for tag in a:tags
        if has_key(tag, 'children')
            call s:SortTags(tag.children, a:comparemethod)
        endif
    endfor
endfunction

" s:CompareByKind() {{{2
function! s:CompareByKind(tag1, tag2)
    let typeinfo = s:compare_typeinfo

    if typeinfo.kinddict[a:tag1.fields.kind] <#
     \ typeinfo.kinddict[a:tag2.fields.kind]
        return -1
    elseif typeinfo.kinddict[a:tag1.fields.kind] >#
         \ typeinfo.kinddict[a:tag2.fields.kind]
        return 1
    else
        " Ignore '~' prefix for C++ destructors to sort them directly under
        " the constructors
        if a:tag1.name[0] ==# '~'
            let name1 = a:tag1.name[1:]
        else
            let name1 = a:tag1.name
        endif
        if a:tag2.name[0] ==# '~'
            let name2 = a:tag2.name[1:]
        else
            let name2 = a:tag2.name
        endif

        if name1 <=# name2
            return -1
        else
            return 1
        endif
    endif
endfunction

" s:CompareByLine() {{{2
function! s:CompareByLine(tag1, tag2)
    return a:tag1.fields.line - a:tag2.fields.line
endfunction

" s:ToggleSort() {{{2
function! s:ToggleSort()
    let fileinfo = s:known_files.getCurrent()
    if empty(fileinfo)
        return
    endif

    let curline = line('.')

    match none

    let s:compare_typeinfo = s:known_types[fileinfo.ftype]

    if has_key(s:compare_typeinfo, 'sort')
        let s:compare_typeinfo.sort = !s:compare_typeinfo.sort
    else
        let g:tagbar_sort = !g:tagbar_sort
    endif

    call fileinfo.sortTags()

    call s:RenderContent()

    execute curline
endfunction

" Display {{{1
" s:RenderContent() {{{2
function! s:RenderContent(...)
    call s:LogDebugMessage('RenderContent called')
    let s:new_window = 0

    if a:0 == 1
        let fileinfo = a:1
    else
        let fileinfo = s:known_files.getCurrent()
    endif

    if empty(fileinfo)
        call s:LogDebugMessage('Empty fileinfo, returning')
        return
    endif

    let tagbarwinnr = bufwinnr('__Tagbar__')

    if &filetype == 'tagbar'
        let in_tagbar = 1
    else
        let in_tagbar = 0
        let prevwinnr = winnr()
        call s:winexec(tagbarwinnr . 'wincmd w')
    endif

    if !empty(s:known_files.getCurrent()) &&
     \ fileinfo.fpath ==# s:known_files.getCurrent().fpath
        " We're redisplaying the same file, so save the view
        call s:LogDebugMessage('Redisplaying file' . fileinfo.fpath)
        let saveline = line('.')
        let savecol  = col('.')
        let topline  = line('w0')
    endif

    let lazyredraw_save = &lazyredraw
    set lazyredraw
    let eventignore_save = &eventignore
    set eventignore=all

    setlocal modifiable

    silent %delete _

    call s:PrintHelp()

    let typeinfo = fileinfo.typeinfo

    " Print tags
    call s:PrintKinds(typeinfo, fileinfo)

    " Delete empty lines at the end of the buffer
    for linenr in range(line('$'), 1, -1)
        if getline(linenr) =~ '^$'
            execute 'silent ' . linenr . 'delete _'
        else
            break
        endif
    endfor

    setlocal nomodifiable

    if !empty(s:known_files.getCurrent()) &&
     \ fileinfo.fpath ==# s:known_files.getCurrent().fpath
        let scrolloff_save = &scrolloff
        set scrolloff=0

        call cursor(topline, 1)
        normal! zt
        call cursor(saveline, savecol)

        let &scrolloff = scrolloff_save
    else
        " Make sure as much of the Tagbar content as possible is shown in the
        " window by jumping to the top after drawing
        execute 1
        call winline()

        " Invalidate highlight cache from old file
        let s:last_highlight_tline = 0
    endif

    let &lazyredraw  = lazyredraw_save
    let &eventignore = eventignore_save

    if !in_tagbar
        call s:winexec(prevwinnr . 'wincmd w')
    endif
endfunction

" s:PrintKinds() {{{2
function! s:PrintKinds(typeinfo, fileinfo)
    call s:LogDebugMessage('PrintKinds called')

    let first_tag = 1

    for kind in a:typeinfo.kinds
        let curtags = filter(copy(a:fileinfo.tags),
                           \ 'v:val.fields.kind ==# kind.short')
        call s:LogDebugMessage('Printing kind: ' . kind.short .
                             \ ', number of (top-level) tags: ' . len(curtags))

        if empty(curtags)
            continue
        endif

        if has_key(a:typeinfo, 'kind2scope') &&
         \ has_key(a:typeinfo.kind2scope, kind.short)
            " Scoped tags
            for tag in curtags
                if g:tagbar_compact && first_tag && s:short_help
                    silent 0put =tag.strfmt()
                else
                    silent  put =tag.strfmt()
                endif

                " Save the current tagbar line in the tag for easy
                " highlighting access
                let curline                   = line('.')
                let tag.tline                 = curline
                let a:fileinfo.tline[curline] = tag

                " Print children
                if tag.isFoldable() && !tag.isFolded()
                    for ckind in a:typeinfo.kinds
                        let childtags = filter(copy(tag.children),
                                          \ 'v:val.fields.kind ==# ckind.short')
                        if len(childtags) > 0
                            " Print 'kind' header of following children, but
                            " only if they are not scope-defining tags (since
                            " those already have an identifier)
                            if !has_key(a:typeinfo.kind2scope, ckind.short)
                                silent put ='    [' . ckind.long . ']'
                                " Add basic tag to allow folding when on the
                                " header line
                                let headertag = s:BaseTag.New(ckind.long)
                                let headertag.parent = tag
                                let headertag.fileinfo = tag.fileinfo
                                let a:fileinfo.tline[line('.')] = headertag
                            endif
                            for childtag in childtags
                                call s:PrintTag(childtag, 1,
                                              \ a:fileinfo, a:typeinfo)
                            endfor
                        endif
                    endfor
                endif

                if !g:tagbar_compact
                    silent put _
                endif

                let first_tag = 0
            endfor
        else
            " Non-scoped tags
            let kindtag = curtags[0].parent

            if kindtag.isFolded()
                let foldmarker = s:icon_closed
            else
                let foldmarker = s:icon_open
            endif

            if g:tagbar_compact && first_tag && s:short_help
                silent 0put =foldmarker . ' ' . kind.long
            else
                silent  put =foldmarker . ' ' . kind.long
            endif

            let curline                   = line('.')
            let kindtag.tline             = curline
            let a:fileinfo.tline[curline] = kindtag

            if !kindtag.isFolded()
                for tag in curtags
                    let str = tag.strfmt()
                    silent put ='  ' . str

                    " Save the current tagbar line in the tag for easy
                    " highlighting access
                    let curline                   = line('.')
                    let tag.tline                 = curline
                    let a:fileinfo.tline[curline] = tag
                    let tag.depth                 = 1
                endfor
            endif

            if !g:tagbar_compact
                silent put _
            endif

            let first_tag = 0
        endif
    endfor
endfunction

" s:PrintTag() {{{2
function! s:PrintTag(tag, depth, fileinfo, typeinfo)
    " Print tag indented according to depth
    silent put =repeat(' ', a:depth * 2) . a:tag.strfmt()

    " Save the current tagbar line in the tag for easy
    " highlighting access
    let curline                   = line('.')
    let a:tag.tline               = curline
    let a:fileinfo.tline[curline] = a:tag

    " Recursively print children
    if a:tag.isFoldable() && !a:tag.isFolded()
        for ckind in a:typeinfo.kinds
            let childtags = filter(copy(a:tag.children),
                                 \ 'v:val.fields.kind ==# ckind.short')
            if len(childtags) > 0
                " Print 'kind' header of following children, but only if they
                " are not scope-defining tags (since those already have an
                " identifier)
                if !has_key(a:typeinfo.kind2scope, ckind.short)
                    silent put ='    ' . repeat(' ', a:depth * 2) .
                              \ '[' . ckind.long . ']'
                    " Add basic tag to allow folding when on the header line
                    let headertag = s:BaseTag.New(ckind.long)
                    let headertag.parent = a:tag
                    let headertag.fileinfo = a:tag.fileinfo
                    let a:fileinfo.tline[line('.')] = headertag
                endif
                for childtag in childtags
                    call s:PrintTag(childtag, a:depth + 1,
                                  \ a:fileinfo, a:typeinfo)
                endfor
            endif
        endfor
    endif
endfunction

" s:PrintHelp() {{{2
function! s:PrintHelp()
    if !g:tagbar_compact && s:short_help
        silent 0put ='\" Press <F1> for help'
        silent  put _
    elseif !s:short_help
        silent 0put ='\" Tagbar keybindings'
        silent  put ='\"'
        silent  put ='\" --------- General ---------'
        silent  put ='\" <Enter> : Jump to tag definition'
        silent  put ='\" p       : As above, but stay in'
        silent  put ='\"           Tagbar window'
        silent  put ='\" <C-N>   : Go to next top-level tag'
        silent  put ='\" <C-P>   : Go to previous top-level tag'
        silent  put ='\" <Space> : Display tag prototype'
        silent  put ='\"'
        silent  put ='\" ---------- Folds ----------'
        silent  put ='\" +, zo   : Open fold'
        silent  put ='\" -, zc   : Close fold'
        silent  put ='\" o, za   : Toggle fold'
        silent  put ='\" *, zR   : Open all folds'
        silent  put ='\" =, zM   : Close all folds'
        silent  put ='\"'
        silent  put ='\" ---------- Misc -----------'
        silent  put ='\" s       : Toggle sort'
        silent  put ='\" x       : Zoom window in/out'
        silent  put ='\" q       : Close window'
        silent  put ='\" <F1>    : Remove help'
        silent  put _
    endif
endfunction

" s:RenderKeepView() {{{2
" The gist of this function was taken from NERDTree by Martin Grenfell.
function! s:RenderKeepView(...)
    if a:0 == 1
        let line = a:1
    else
        let line = line('.')
    endif

    let curcol  = col('.')
    let topline = line('w0')

    call s:RenderContent()

    let scrolloff_save = &scrolloff
    set scrolloff=0

    call cursor(topline, 1)
    normal! zt
    call cursor(line, curcol)

    let &scrolloff = scrolloff_save

    redraw
endfunction

" User actions {{{1
" s:HighlightTag() {{{2
function! s:HighlightTag(openfolds, ...)
    let tagline = 0

    if a:0 > 0
        let tag = s:GetNearbyTag(1, a:1)
    else
        let tag = s:GetNearbyTag(1)
    endif
    if !empty(tag)
        let tagline = tag.tline
    endif

    " Don't highlight the tag again if it's the same one as last time.
    " This prevents the Tagbar window from jumping back after scrolling with
    " the mouse.
    if tagline == s:last_highlight_tline
        return
    else
        let s:last_highlight_tline = tagline
    endif

    let tagbarwinnr = bufwinnr('__Tagbar__')
    if tagbarwinnr == -1
        return
    endif
    let prevwinnr   = winnr()
    call s:winexec(tagbarwinnr . 'wincmd w')

    match none

    " No tag above cursor position so don't do anything
    if tagline == 0
        call s:winexec(prevwinnr . 'wincmd w')
        redraw
        return
    endif

    if g:tagbar_autoshowtag || a:openfolds
        call s:OpenParents(tag)
    endif

    " Check whether the tag is inside a closed fold and highlight the parent
    " instead in that case
    let tagline = tag.getClosedParentTline()

    " Go to the line containing the tag
    execute tagline

    " Make sure the tag is visible in the window
    call winline()

    let foldpat = '[' . s:icon_open . s:icon_closed . ' ]'
    let pattern = '/^\%' . tagline . 'l\s*' . foldpat . '[-+# ]\zs[^( ]\+\ze/'
    execute 'match TagbarHighlight ' . pattern

    if a:0 == 0 " no line explicitly given, so assume we were in the file window
        call s:winexec(prevwinnr . 'wincmd w')
    endif

    redraw
endfunction

" s:JumpToTag() {{{2
function! s:JumpToTag(stay_in_tagbar)
    let taginfo = s:GetTagInfo(line('.'), 1)

    let autoclose = w:autoclose

    if empty(taginfo) || !taginfo.isNormalTag()
        return
    endif

    let tagbarwinnr = winnr()

    " This elaborate construct will try to switch to the correct
    " buffer/window; if the buffer isn't currently shown in a window it will
    " open it in the first window with a non-special buffer in it
    call s:winexec('wincmd p')
    let filebufnr = bufnr(taginfo.fileinfo.fpath)
    if bufnr('%') != filebufnr
        let filewinnr = bufwinnr(filebufnr)
        if filewinnr != -1
            call s:winexec(filewinnr . 'wincmd w')
        else
            for i in range(1, winnr('$'))
                call s:winexec(i . 'wincmd w')
                if &buftype == ''
                    execute 'buffer ' . filebufnr
                    break
                endif
            endfor
        endif
        " To make ctrl-w_p work we switch between the Tagbar window and the
        " correct window once
        call s:winexec(tagbarwinnr . 'wincmd w')
        call s:winexec('wincmd p')
    endif

    " Mark current position so it can be jumped back to
    mark '

    " Jump to the line where the tag is defined. Don't use the search pattern
    " since it doesn't take the scope into account and thus can fail if tags
    " with the same name are defined in different scopes (e.g. classes)
    execute taginfo.fields.line

    " If the file has been changed but not saved, the tag may not be on the
    " saved line anymore, so search for it in the vicinity of the saved line
    if match(getline('.'), taginfo.pattern) == -1
        let interval = 1
        let forward  = 1
        while search(taginfo.pattern, 'W' . forward ? '' : 'b') == 0
            if !forward
                if interval > line('$')
                    break
                else
                    let interval = interval * 2
                endif
            endif
            let forward = !forward
        endwhile
    endif

    " If the tag is on a different line after unsaved changes update the tag
    " and file infos/objects
    let curline = line('.')
    if taginfo.fields.line != curline
        let taginfo.fields.line = curline
        let taginfo.fileinfo.fline[curline] = taginfo
    endif

    " Center the tag in the window and jump to the correct column if available
    normal! z.
    call cursor(taginfo.fields.line, taginfo.fields.column)

    if foldclosed('.') != -1
        .foldopen!
    endif

    redraw

    if a:stay_in_tagbar
        call s:HighlightTag(0)
        call s:winexec(tagbarwinnr . 'wincmd w')
    elseif g:tagbar_autoclose || autoclose
        call s:CloseWindow()
    else
        call s:HighlightTag(0)
    endif
endfunction

" s:ShowPrototype() {{{2
function! s:ShowPrototype()
    let taginfo = s:GetTagInfo(line('.'), 1)

    if empty(taginfo)
        return
    endif

    echo taginfo.getPrototype()
endfunction

" s:ToggleHelp() {{{2
function! s:ToggleHelp()
    let s:short_help = !s:short_help

    " Prevent highlighting from being off after adding/removing the help text
    match none

    call s:RenderContent()

    execute 1
    redraw
endfunction

" s:GotoNextToplevelTag() {{{2
function! s:GotoNextToplevelTag(direction)
    let curlinenr = line('.')
    let newlinenr = line('.')

    if a:direction == 1
        let range = range(line('.') + 1, line('$'))
    else
        let range = range(line('.') - 1, 1, -1)
    endif

    for tmplinenr in range
        let taginfo = s:GetTagInfo(tmplinenr, 0)

        if empty(taginfo)
            continue
        elseif empty(taginfo.parent)
            let newlinenr = tmplinenr
            break
        endif
    endfor

    if curlinenr != newlinenr
        execute newlinenr
        call winline()
    endif

    redraw
endfunction

" Folding {{{1
" s:OpenFold() {{{2
function! s:OpenFold()
    let fileinfo = s:known_files.getCurrent()
    if empty(fileinfo)
        return
    endif

    let curline = line('.')

    let tag = s:GetTagInfo(curline, 0)
    if empty(tag)
        return
    endif

    call tag.openFold()

    call s:RenderKeepView()
endfunction

" s:CloseFold() {{{2
function! s:CloseFold()
    let fileinfo = s:known_files.getCurrent()
    if empty(fileinfo)
        return
    endif

    match none

    let curline = line('.')

    let curtag = s:GetTagInfo(curline, 0)
    if empty(curtag)
        return
    endif

    let newline = curtag.closeFold()

    call s:RenderKeepView(newline)
endfunction

" s:ToggleFold() {{{2
function! s:ToggleFold()
    let fileinfo = s:known_files.getCurrent()
    if empty(fileinfo)
        return
    endif

    match none

    let curtag = s:GetTagInfo(line('.'), 0)
    if empty(curtag)
        return
    endif

    let newline = line('.')

    if curtag.isKindheader()
        call curtag.toggleFold()
    elseif curtag.isFoldable()
        if curtag.isFolded()
            call curtag.openFold()
        else
            let newline = curtag.closeFold()
        endif
    else
        let newline = curtag.closeFold()
    endif

    call s:RenderKeepView(newline)
endfunction

" s:SetFoldLevel() {{{2
function! s:SetFoldLevel(level, force)
    if a:level < 0
        echoerr 'Foldlevel can''t be negative'
        return
    endif

    let fileinfo = s:known_files.getCurrent()
    if empty(fileinfo)
        return
    endif

    call s:SetFoldLevelRecursive(fileinfo, fileinfo.tags, a:level)

    let typeinfo = fileinfo.typeinfo

    " Apply foldlevel to 'kind's
    if a:level == 0
        for kind in typeinfo.kinds
            call fileinfo.closeKindFold(kind)
        endfor
    else
        for kind in typeinfo.kinds
            if a:force || !kind.fold
                call fileinfo.openKindFold(kind)
            endif
        endfor
    endif

    let fileinfo.foldlevel = a:level

    call s:RenderContent()
endfunction

" s:SetFoldLevelRecursive() {{{2
" Apply foldlevel to normal tags
function! s:SetFoldLevelRecursive(fileinfo, tags, level)
    for tag in a:tags
        if tag.depth >= a:level
            call tag.setFolded(1)
        else
            call tag.setFolded(0)
        endif

        if has_key(tag, 'children')
            call s:SetFoldLevelRecursive(a:fileinfo, tag.children, a:level)
        endif
    endfor
endfunction

" s:OpenParents() {{{2
function! s:OpenParents(...)
    let tagline = 0

    if a:0 == 1
        let tag = a:1
    else
        let tag = s:GetNearbyTag(1)
    endif

    call tag.openParents()

    call s:RenderKeepView()
endfunction

" Helper functions {{{1
" s:AutoUpdate() {{{2
function! s:AutoUpdate(fname)
    call s:LogDebugMessage('AutoUpdate called on ' . a:fname)

    " Get the filetype of the file we're about to process
    let bufnr = bufnr(a:fname)
    let ftype = getbufvar(bufnr, '&filetype')

    " Don't do anything if we're in the tagbar window
    if ftype == 'tagbar'
        call s:LogDebugMessage('Tagbar window not open or in Tagbar window')
        return
    endif

    " Only consider the main filetype in cases like 'python.django'
    let sftype = get(split(ftype, '\.'), 0, '')
    call s:LogDebugMessage("Vim filetype: '" . ftype . "', " .
                         \ "sanitized filetype: '" . sftype . "'")

    " Don't do anything if the file isn't supported
    if !s:IsValidFile(a:fname, sftype)
        call s:LogDebugMessage('Not a valid file, stopping processing')
        return
    endif

    let updated = 0

    " Process the file if it's unknown or the information is outdated
    " Also test for entries that exist but are empty, which will be the case
    " if there was an error during the ctags execution
    if s:known_files.has(a:fname) && !empty(s:known_files.get(a:fname))
        if s:known_files.get(a:fname).mtime != getftime(a:fname)
            call s:LogDebugMessage('File data outdated, updating ' . a:fname)
            call s:ProcessFile(a:fname, sftype)
            let updated = 1
        endif
    elseif !s:known_files.has(a:fname)
        call s:LogDebugMessage('New file, processing ' . a:fname)
        call s:ProcessFile(a:fname, sftype)
        let updated = 1
    endif

    let fileinfo = s:known_files.get(a:fname)

    " If we don't have an entry for the file by now something must have gone
    " wrong, so don't change the tagbar content
    if empty(fileinfo)
        call s:LogDebugMessage('fileinfo empty after processing ' . a:fname)
        return
    endif

    " Display the tagbar content if the tags have been updated or a different
    " file is being displayed
    if bufwinnr('__Tagbar__') != -1 &&
     \ (s:new_window || updated || a:fname != s:known_files.getCurrent().fpath)
        call s:RenderContent(fileinfo)
    endif

    " Call setCurrent after rendering so RenderContent can check whether the
    " same file is redisplayed
    if !empty(fileinfo)
        call s:LogDebugMessage('Setting current file to ' . a:fname)
        call s:known_files.setCurrent(fileinfo)
    endif

    call s:HighlightTag(0)
    call s:LogDebugMessage('AutoUpdate finished successfully')
endfunction

" s:CheckMouseClick() {{{2
function! s:CheckMouseClick()
    let line   = getline('.')
    let curcol = col('.')

    if (match(line, s:icon_open . '[-+ ]') + 1) == curcol
        call s:CloseFold()
    elseif (match(line, s:icon_closed . '[-+ ]') + 1) == curcol
        call s:OpenFold()
    elseif g:tagbar_singleclick
        call s:JumpToTag(0)
    endif
endfunction

" s:DetectFiletype() {{{2
function! s:DetectFiletype(bufnr)
    " Filetype has already been detected for loaded buffers, but not
    " necessarily for unloaded ones
    let ftype = getbufvar(a:bufnr, '&filetype')

    if bufloaded(a:bufnr)
        return ftype
    endif

    if ftype != ''
        return ftype
    endif

    " Unloaded buffer with non-detected filetype, need to detect filetype
    " manually
    let bufname = bufname(a:bufnr)

    let eventignore_save = &eventignore
    set eventignore=FileType
    let filetype_save = &filetype

    exe 'doautocmd filetypedetect BufRead ' . bufname
    let ftype = &filetype

    let &filetype = filetype_save
    let &eventignore = eventignore_save

    return ftype
endfunction

" s:EscapeCtagsCmd() {{{2
" Assemble the ctags command line in a way that all problematic characters are
" properly escaped and converted to the system's encoding
" Optional third parameter is a file name to run ctags on
function! s:EscapeCtagsCmd(ctags_bin, args, ...)
    call s:LogDebugMessage('EscapeCtagsCmd called')
    call s:LogDebugMessage('ctags_bin: ' . a:ctags_bin)
    call s:LogDebugMessage('ctags_args: ' . a:args)

    if exists('+shellslash')
        let shellslash_save = &shellslash
        set noshellslash
    endif

    if a:0 == 1
        let fname = shellescape(a:1)
    else
        let fname = ''
    endif

    let ctags_cmd = shellescape(a:ctags_bin) . ' ' . a:args . ' ' . fname

    " Stupid cmd.exe quoting
    if &shell =~ 'cmd\.exe'
        let ctags_cmd = substitute(ctags_cmd, '\(&\|\^\)', '^\0', 'g')
    endif

    if exists('+shellslash')
        let &shellslash = shellslash_save
    endif

    " Needed for cases where 'encoding' is different from the system's
    " encoding
    if g:tagbar_systemenc != &encoding
        let ctags_cmd = iconv(ctags_cmd, &encoding, g:tagbar_systemenc)
    elseif $LANG != ''
        let ctags_cmd = iconv(ctags_cmd, &encoding, $LANG)
    endif

    call s:LogDebugMessage('Escaped ctags command: ' . ctags_cmd)

    if ctags_cmd == ''
        echoerr 'Tagbar: Encoding conversion failed!'
              \ 'Please make sure your system is set up correctly'
              \ 'and that Vim is compiled with the "iconv" feature.'
    endif

    return ctags_cmd
endfunction

" s:ExecuteCtags() {{{2
" Execute ctags with necessary shell settings
" Partially based on the discussion at
" http://vim.1045645.n5.nabble.com/bad-default-shellxquote-in-Widows-td1208284.html
function! s:ExecuteCtags(ctags_cmd)
    if exists('+shellslash')
        let shellslash_save = &shellslash
        set noshellslash
    endif

    if &shell =~ 'cmd\.exe'
        let shellxquote_save = &shellxquote
        set shellxquote=\"
        let shellcmdflag_save = &shellcmdflag
        set shellcmdflag=/s\ /c
    endif

    let ctags_output = system(a:ctags_cmd)

    if &shell =~ 'cmd\.exe'
        let &shellxquote  = shellxquote_save
        let &shellcmdflag = shellcmdflag_save
    endif

    if exists('+shellslash')
        let &shellslash = shellslash_save
    endif

    return ctags_output
endfunction

" s:GetNearbyTag() {{{2
" Get the tag info for a file near the cursor in the current file
function! s:GetNearbyTag(all, ...)
    let fileinfo = s:known_files.getCurrent()
    if empty(fileinfo)
        return {}
    endif

    let typeinfo = fileinfo.typeinfo
    if a:0 > 0
        let curline = a:1
    else
        let curline = line('.')
    endif
    let tag = {}

    " If a tag appears in a file more than once (for example namespaces in
    " C++) only one of them has a 'tline' entry and can thus be highlighted.
    " The only way to solve this would be to go over the whole tag list again,
    " making everything slower. Since this should be a rare occurence and
    " highlighting isn't /that/ important ignore it for now.
    for line in range(curline, 1, -1)
        if has_key(fileinfo.fline, line)
            let curtag = fileinfo.fline[line]
            if a:all || typeinfo.getKind(curtag.fields.kind).stl
                let tag = curtag
                break
            endif
        endif
    endfor

    return tag
endfunction

" s:GetTagInfo() {{{2
" Return the info dictionary of the tag on the specified line. If the line
" does not contain a valid tag (for example because it is empty or only
" contains a pseudo-tag) return an empty dictionary.
function! s:GetTagInfo(linenr, ignorepseudo)
    let fileinfo = s:known_files.getCurrent()

    if empty(fileinfo)
        return {}
    endif

    " Don't do anything in empty and comment lines
    let curline = getline(a:linenr)
    if curline =~ '^\s*$' || curline[0] == '"'
        return {}
    endif

    " Check if there is a tag on the current line
    if !has_key(fileinfo.tline, a:linenr)
        return {}
    endif

    let taginfo = fileinfo.tline[a:linenr]

    " Check if the current tag is not a pseudo-tag
    if a:ignorepseudo && taginfo.isPseudoTag()
        return {}
    endif

    return taginfo
endfunction

" s:IsValidFile() {{{2
function! s:IsValidFile(fname, ftype)
    call s:LogDebugMessage('Checking if file is valid: ' . a:fname)

    if a:fname == '' || a:ftype == ''
        call s:LogDebugMessage('Empty filename or type')
        return 0
    endif

    if !filereadable(a:fname)
        call s:LogDebugMessage('File not readable')
        return 0
    endif

    if !has_key(s:known_types, a:ftype)
        if exists('g:tagbar_type_' . a:ftype)
            " Filetype definition must have been specified in an 'ftplugin'
            " file, so load it now
            call s:LoadUserTypeDefs(a:ftype)
        else
            call s:LogDebugMessage('Unsupported filetype: ' . a:ftype)
            return 0
        endif
    endif

    return 1
endfunction

" s:QuitIfOnlyWindow() {{{2
function! s:QuitIfOnlyWindow()
    " Check if there is more than window
    if winbufnr(2) == -1
        " Check if there is more than one tab page
        if tabpagenr('$') == 1
            " Before quitting Vim, delete the tagbar buffer so that
            " the '0 mark is correctly set to the previous buffer.
            bdelete
            quitall
        else
            close
        endif
    endif
endfunction

" s:winexec() {{{2
function! s:winexec(cmd)
    call s:LogDebugMessage("Executing without autocommands: " . a:cmd)

    let eventignore_save = &eventignore
    set eventignore=BufEnter

    execute a:cmd

    let &eventignore = eventignore_save
endfunction

" TagbarBalloonExpr() {{{2
function! TagbarBalloonExpr()
    let taginfo = s:GetTagInfo(v:beval_lnum, 1)

    if empty(taginfo)
        return
    endif

    return taginfo.getPrototype()
endfunction

" TagbarGenerateStatusline() {{{2
function! TagbarGenerateStatusline()
    if g:tagbar_sort
        let text = '[Name]'
    else
        let text = '[Order]'
    endif

    if !empty(s:known_files.getCurrent())
        let filename = fnamemodify(s:known_files.getCurrent().fpath, ':t')
        let text .= ' ' . filename
    endif

    return text
endfunction

" Debugging {{{1
" s:StartDebug() {{{2
function! s:StartDebug(filename)
    if empty(a:filename)
        let s:debug_file = 'tagbardebug.log'
    else
        let s:debug_file = a:filename
    endif

    " Empty log file
    exe 'redir! > ' . s:debug_file
    redir END

    " Check whether the log file could be created
    if !filewritable(s:debug_file)
        echomsg 'Tagbar: Unable to create log file ' . s:debug_file
        let s:debug_file = ''
        return
    endif

    let s:debug = 1
endfunction

" s:StopDebug() {{{2
function! s:StopDebug()
    let s:debug = 0
    let s:debug_file = ''
endfunction

" s:LogDebugMessage() {{{2
function! s:LogDebugMessage(msg)
    if s:debug
        exe 'redir >> ' . s:debug_file
        silent echon strftime('%H:%M:%S') . ': ' . a:msg . "\n"
        redir END
    endif
endfunction

" Autoload functions {{{1

" Wrappers {{{2
function! tagbar#ToggleWindow()
    call s:ToggleWindow()
endfunction

function! tagbar#OpenWindow(...)
    let flags = a:0 > 0 ? a:1 : ''
    call s:OpenWindow(flags)
endfunction

function! tagbar#CloseWindow()
    call s:CloseWindow()
endfunction

function! tagbar#SetFoldLevel(level, force)
    call s:SetFoldLevel(a:level, a:force)
endfunction

function! tagbar#OpenParents()
    call s:OpenParents()
endfunction

function! tagbar#StartDebug(...)
    let filename = a:0 > 0 ? a:1 : ''
    call s:StartDebug(filename)
endfunction

function! tagbar#StopDebug()
    call s:StopDebug()
endfunction

function! tagbar#RestoreSession()
    call s:RestoreSession()
endfunction
" }}}2

" tagbar#getusertypes() {{{2
function! tagbar#getusertypes()
    redir => defs
    silent execute 'let g:'
    redir END

    let deflist = split(defs, '\n')
    call map(deflist, 'substitute(v:val, ''^\S\+\zs.*'', "", "")')
    call filter(deflist, 'v:val =~ "^tagbar_type_"')

    let defdict = {}
    for defstr in deflist
        let type = substitute(defstr, '^tagbar_type_', '', '')
        let defdict[type] = g:{defstr}
    endfor

    return defdict
endfunction

" tagbar#autoopen() {{{2
" Automatically open Tagbar if one of the open buffers contains a supported
" file
function! tagbar#autoopen(...)
    call s:LogDebugMessage('tagbar#autoopen called on ' . bufname('%'))
    let always = a:0 > 0 ? a:1 : 1

    call s:Init()

    for bufnr in range(1, bufnr('$'))
        if buflisted(bufnr) && (always || bufwinnr(bufnr) != -1)
            let ftype = s:DetectFiletype(bufnr)
            if s:IsValidFile(bufname(bufnr), ftype)
                call s:OpenWindow('')
                call s:LogDebugMessage('tagbar#autoopen finished ' .
                                     \ 'after finding valid file')
                return
            endif
        endif
    endfor

    call s:LogDebugMessage('tagbar#autoopen finished ' .
                         \ 'without finding valid file')
endfunction

" tagbar#currenttag() {{{2
function! tagbar#currenttag(fmt, default, ...)
    if a:0 > 0
        " also test for non-zero value for backwards compatibility
        let longsig  = a:1 =~# 's' || (type(a:1) == type(0) && a:1 != 0)
        let fullpath = a:1 =~# 'f'
    else
        let longsig  = 0
        let fullpath = 0
    endif

    if !s:Init()
        return ''
    endif

    let tag = s:GetNearbyTag(0)

    if !empty(tag)
        return printf(a:fmt, tag.str(longsig, fullpath))
    else
        return a:default
    endif
endfunction

" tagbar#gettypeconfig() {{{2
function! tagbar#gettypeconfig(type)
    if !s:Init()
        return ''
    endif

    let typeinfo = get(s:known_types, a:type, {})

    if empty(typeinfo)
        echoerr 'Unknown type ' . a:type . '!'
        return
    endif

    let output = "let g:tagbar_type_" . a:type . " = {\n"

    let output .= "    \\ 'kinds' : [\n"
    for kind in typeinfo.kinds
        let output .= "        \\ '" . kind.short . ":" . kind.long
        if kind.fold || !kind.stl
            if kind.fold
                let output .= ":1"
            else
                let output .= ":0"
            endif
        endif
        if !kind.stl
            let output .= ":0"
        endif
        let output .= "',\n"
    endfor
    let output .= "    \\ ],\n"

    let output .= "\\ }"

    silent put =output
endfunction

" Modeline {{{1
" vim: ts=8 sw=4 sts=4 et foldenable foldmethod=marker foldcolumn=1
doc/tagbar.txt	[[[1
1148
*tagbar.txt*    Display tags of a file ordered by scope

Author:         Jan Larres <jan@majutsushi.net>
Licence:        Vim licence, see |license|
Homepage:       http://majutsushi.github.com/tagbar/
Version:        2.4

==============================================================================
Contents                                            *tagbar* *tagbar-contents*

         1. Intro ........................... |tagbar-intro|
              Pseudo-tags ................... |tagbar-pseudotags|
              Supported features ............ |tagbar-features|
              Other ctags-compatible programs |tagbar-other|
         2. Requirements .................... |tagbar-requirements|
         3. Installation .................... |tagbar-installation|
         4. Usage ........................... |tagbar-usage|
              Commands ...................... |tagbar-commands|
              Key mappings .................. |tagbar-keys|
         5. Configuration ................... |tagbar-configuration|
              Highlight colours ............. |tagbar-highlight|
              Automatically opening Tagbar .. |tagbar-autoopen|
              Show current tag in statusline  |tagbar-statusline|
         6. Extending Tagbar ................ |tagbar-extend|
         7. Troubleshooting & Known issues .. |tagbar-issues|
         8. History ......................... |tagbar-history|
         9. Todo ............................ |tagbar-todo|
        10. Credits ......................... |tagbar-credits|

==============================================================================
1. Intro                                                        *tagbar-intro*

Tagbar is a plugin for browsing the tags of source code files. It provides a
sidebar that displays the ctags-generated tags of the current file, ordered by
their scope. This means that for example methods in C++ are displayed under
the class they are defined in.

Let's say we have the following code inside of a C++ file:
>
        namespace {
            char a;

            class Foo
            {
            public:
                Foo();
                ~Foo();
            private:
                int var;
            };
        };
<
Then Tagbar would display the tag information like so:
>
        __anon1* : namespace
          Foo : class
           +Foo()
           +~Foo()
           -var
          a
<
This example shows several important points. First, the tags are listed
indented below the scope they are defined in. Second, the type of a scope is
listed after its name and a colon. Third, tags for which the access/visibility
information is known are prefixed with a symbol indicating that.

------------------------------------------------------------------------------
PSEUDO-TAGS                                                *tagbar-pseudotags*

The example also introduces the concept of "pseudo-tags". Pseudo-tags are tags
that are not explicitly defined in the file but have children in it. In this
example the namespace doesn't have a name and thus ctags doesn't generate a
tag for it, but since it has children it still needs to be displayed using an
auto-generated name.

Another case where pseudo-tags appear is in C++ implementation files. Since
classes are usually defined in a header file but the member methods and
variables in the implementation file the class itself won't generate a tag
in that file.

Since pseudo-tags don't really exist they cannot be jumped to from the Tagbar
window.

Pseudo-tags are denoted with an asterisk ('*') at the end of their name.

------------------------------------------------------------------------------
SUPPORTED FEATURES                                           *tagbar-features*

The following features are supported by Tagbar:

  - Display tags under their correct scope.
  - Automatically update the tags when switching between buffers and editing
    files.
  - Display visibility information of tags if available.
  - Highlight the tag near the cursor while editing files.
  - Jump to a tag from the Tagbar window.
  - Display the complete prototype of a tag.
  - Tags can be sorted either by name or order of appearance in the file.
  - Scopes can be folded to hide uninteresting information.
  - Supports all of the languages that ctags does, i.e. Ant, Assembler, ASP,
    Awk, Basic, BETA, C, C++, C#, COBOL, DosBatch, Eiffel, Erlang, Flex,
    Fortran, HTML, Java, JavaScript, Lisp, Lua, Make, MatLab, OCaml, Pascal,
    Perl, PHP, Python, REXX, Ruby, Scheme, Shell script, SLang, SML, SQL, Tcl,
    Tex, Vera, Verilog, VHDL, Vim and YACC.
  - Can be extended to support arbitrary new types.

------------------------------------------------------------------------------
OTHER CTAGS-COMPATIBLE PROGRAMS                                 *tagbar-other*

Tagbar theoretically also supports filetype-specific programs that can output
tag information that is compatible with ctags. However due to potential
incompatibilities this may not always completely work. Tagbar has been tested
with doctorjs/jsctags and will use that if present, other programs require
some configuration (see |tagbar-extend|). If a program does not work even with
correct configuration please contact me.

Note: Please check |tagbar-issues| for some possible issues with jsctags.

==============================================================================
2. Requirements                                          *tagbar-requirements*

The following requirements have to be met in order to be able to use tagbar:

  - Vim 7.0 or higher. Older versions will not work since Tagbar uses data
    structures that were only introduced in Vim 7.
  - Exuberant ctags 5.5 or higher. Ctags is the program that generates the
    tag information that Tagbar uses. It is shipped with most Linux
    distributions, otherwise it can be downloaded from the following
    website:

        http://ctags.sourceforge.net/

    Tagbar will work on any platform that ctags runs on -- this includes
    UNIX derivatives, Mac OS X and Windows. Note that other versions like
    GNU ctags will not work.
    Tagbar generates the tag information by itself and doesn't need (or use)
    already existing tag files.
  - File type detection must be turned on in vim. This can be done with the
    following command in your vimrc:
>
        filetype on
<
    See |filetype| for more information.
  - Tagbar will not work in |restricted-mode| or with 'compatible' set.

==============================================================================
3. Installation                                          *tagbar-installation*

Use the normal Vimball install method for installing tagbar.vba:
>
        vim tagbar.vba
        :so %
        :q
<
Alternatively you can clone the git repository and then add the path to
'runtimepath' or use the pathogen plugin. Don't forget to run |:helptags|.

If the ctags executable is not installed in one of the directories in your
$PATH environment variable you have to set the g:tagbar_ctags_bin variable,
see |g:tagbar_ctags_bin|.

==============================================================================
4. Usage                                                        *tagbar-usage*

There are essentially two ways to use Tagbar:

  1. Have it running all the time in a window on the side of the screen. In
     this case Tagbar will update its contents whenever the source file is
     changed and highlight the tag the cursor is currently on in the file. If
     a tag is selected in Tagbar the file window will jump to the tag and the
     Tagbar window will stay open. |g:tagbar_autoclose| has to be unset for
     this mode.
  2. Only open Tagbar when you want to jump to a specific tag and have it
     close automatically once you have selected one. This can be useful for
     example for small screens where a permanent window would take up too much
     space. You have to set the option |g:tagbar_autoclose| in this case. The
     cursor will also automatically jump to the Tagbar window when opening it.

Opening and closing the Tagbar window~
Use |:TagbarOpen| or |:TagbarToggle| to open the Tagbar window if it is
closed. By default the window is opened on the right side, set the option
|g:tagbar_left| to open it on the left instead. If the window is already open,
|:TagbarOpen| will jump to it and |:TagbarToggle| will close it again.
|:TagbarClose| will simply close the window if it is open.

It is probably a good idea to assign a key to these commands. For example, put
this into your |vimrc|:
>
        nnoremap <silent> <F9> :TagbarToggle<CR>
<
You can then open and close Tagbar by simply pressing the <F9> key.

You can also use |:TagbarOpenAutoClose| to open the Tagbar window, jump to it
and have it close automatically on tag selection regardless of the
|g:tagbar_autoclose| setting.

Jumping to tags~
When you're inside the Tagbar window you can jump to the definition of a tag
by moving the cursor to a tag and pressing <Enter> or double-clicking on it
with the mouse. The source file will then move to the definition and put the
cursor in the corresponding line. This won't work for pseudo-tags.

Sorting~
You can sort the tags in the Tagbar window in two ways: by name or by file
order. Sorting them by name simply displays the tags in their alphabetical
order under their corresponding scope. Sorting by file order means that the
tags keep the order they have in the source file, but are still associated
with the correct scope. You can change the sort order by pressing the "s" key
in the Tagbar window. The current sort order is displayed in the statusline of
the Tagbar window.

Folding~
The displayed scopes (and unscoped types) can be folded to hide uninteresting
information. Mappings similar to Vim's built-in ones are provided. Folds can
also be opened and closed by clicking on the fold icon with the mouse.

Highlighting the current tag~
When the Tagbar window is open the current tag will automatically be
highlighted in it after a short pause if the cursor is not moving. The length
of this pause is determined by the 'updatetime' option. If you want to make
that pause shorter you can change the option, but don't set it too low or
strange things will happen. This is unfortunately unavoidable.

Displaying the prototype of a tag~
Tagbar can display the prototype of a tag. More precisely it can display the
line in which the tag is defined. This can be done by either pressing <Space>
when on a tag or hovering over a tag with the mouse. In the former case the
prototype will be displayed in the command line |Command-line|, in the latter
case it will be displayed in a pop-up window. The prototype will also be
displayed when the cursor stays on a tag for 'updatetime' milliseconds.

------------------------------------------------------------------------------
COMMANDS                                                     *tagbar-commands*

:TagbarOpen [{flags}]                                            *:TagbarOpen*
    Open the Tagbar window if it is closed.

    Additional behaviour can be specified with the optional {flags} argument.
    It is a string which can contain these character flags:
    'f'   Jump to Tagbar window when opening (just as if |g:tagbar_autofocus|
          were set to 1)
    'j'   Jump to Tagbar window if already open
    'c'   Close Tagbar on tag selection (just as if |g:tagbar_autoclose| were
          set to 1, but doesn't imply 'f')

    For example, the following command would always jump to the Tagbar window,
    opening it first if necessary, but keep it open after selecting a tag
    (unless |g:tagbar_autoclose| is set): >
        :TagbarOpen fj
<
:TagbarClose                                                    *:TagbarClose*
    Close the Tagbar window if it is open.

:TagbarToggle                                                  *:TagbarToggle*
    Open the Tagbar window if it is closed or close it if it is open.

:TagbarOpenAutoClose                                    *:TagbarOpenAutoClose*
    Open the Tagbar window, jump to it and close it on tag selection. This is
    an alias for ":TagbarOpen fc".

:TagbarSetFoldlevel[!] {number}                          *:TagbarSetFoldlevel*
    Set the foldlevel of the tags of the current file to {number}. The
    foldlevel of tags in other files remains unaffected. Works in the same way
    as 'foldlevel'. Folds that are specified to be closed by default in the
    type configuration will not be opened, use a "!" to force applying the new
    foldlevel to those folds as well.

:TagbarShowTag                                                *:TagbarShowTag*
    Open the parent folds of the current tag in the file window as much as
    needed for the tag to be visible in the Tagbar window.

:TagbarGetTypeConfig {filetype}                         *:TagbarGetTypeConfig*
    Paste the Tagbar configuration of the vim filetype {filetype} at the
    current cursor position (provided that filetype is supported by Tagbar)
    for easy customization. The configuration will be ready to use as is but
    will only contain the "kinds" entry as that is the only one that really
    makes sense to customize. See |tagbar-extend| for more information about
    type configurations.

:TagbarDebug [logfile]                                          *:TagbarDebug*
    Start debug mode. This will write debug messages to file [logfile] while
    using Tagbar. If no argument is given "tagbardebug.log" in the current
    directory is used. Note: an existing file will be overwritten!

:TagbarDebugEnd                                              *:TagbarDebugEnd*
    End debug mode, debug messages will no longer be written to the logfile.

------------------------------------------------------------------------------
KEY MAPPINGS                                                     *tagbar-keys*

The following mappings are valid in the Tagbar window:

<F1>          Display key mapping help.
<CR>/<Enter>  Jump to the tag under the cursor. Doesn't work for pseudo-tags
              or generic headers.
p             Jump to the tag under the cursor, but stay in the Tagbar window.
<LeftMouse>   When on a fold icon, open or close the fold depending on the
              current state.
<2-LeftMouse> Same as <CR>. See |g:tagbar_singleclick| if you want to use a
              single- instead of a double-click.
<Space>       Display the prototype of the current tag (i.e. the line defining
              it) in the command line.
+/zo          Open the fold under the cursor.
-/zc          Close the fold under the cursor or the current one if there is
              no fold under the cursor.
o/za          Toggle the fold under the cursor or the current one if there is
              no fold under the cursor.
*/zR          Open all folds by setting foldlevel to 99.
=/zM          Close all folds by setting foldlevel to 0.
<C-N>         Go to the next top-level tag.
<C-P>         Go to the previous top-level tag.
s             Toggle sort order between name and file order.
x             Toggle zooming the window.
q             Close the Tagbar window.

==============================================================================
5. Configuration                                        *tagbar-configuration*

                                                          *g:tagbar_ctags_bin*
g:tagbar_ctags_bin~
Default: empty

Use this option to specify the location of your ctags executable. Only needed
if it is not in one of the directories in your $PATH environment variable.

Example:
>
        let g:tagbar_ctags_bin = 'C:\Ctags5.8\ctags.exe'
<

                                                               *g:tagbar_left*
g:tagbar_left~
Default: 0

By default the Tagbar window will be opened on the right-hand side of vim. Set
this option to open it on the left instead.

Example:
>
        let g:tagbar_left = 1
<

                                                              *g:tagbar_width*
g:tagbar_width~
Default: 40

Width of the Tagbar window in characters.

Example:
>
        let g:tagbar_width = 30
<

                                                          *g:tagbar_autoclose*
g:tagbar_autoclose~
Default: 0

If you set this option the Tagbar window will automatically close when you
jump to a tag. This implies |g:tagbar_autofocus|.

Example:
>
        let g:tagbar_autoclose = 1
<

                                                          *g:tagbar_autofocus*
g:tagbar_autofocus~
Default: 0

If you set this option the cursor will move to the Tagbar window when it is
opened.

Example:
>
        let g:tagbar_autofocus = 1
<

                                                               *g:tagbar_sort*
g:tagbar_sort~
Default: 1

If this option is set the tags are sorted according to their name. If it is
unset they are sorted according to their order in the source file. Note that
in the second case Pseudo-tags are always sorted before normal tags of the
same kind since they don't have a real position in the file.

Example:
>
        let g:tagbar_sort = 0
<

                                                            *g:tagbar_compact*
g:tagbar_compact~
Default: 0

Setting this option will result in Tagbar omitting the short help at the
top of the window and the blank lines in between top-level scopes in order to
save screen real estate.

Example:
>
        let g:tagbar_compact = 1
<

                                                             *g:tagbar_expand*
g:tagbar_expand~
Default: 0

If this option is set the Vim window will be expanded by the width of the
Tagbar window if using a GUI version of Vim.

Example:
>
        let g:tagbar_expand = 1
<

                                                        *g:tagbar_singleclick*
g:tagbar_singleclick~
Default: 0

If this option is set then a single- instead of a double-click is used to jump
to the tag definition.

Example:
>
        let g:tagbar_singleclick = 1
<

                                                          *g:tagbar_foldlevel*
g:tagbar_foldlevel~
Default: 99

The initial foldlevel for folds in the Tagbar window. Folds with a level
higher than this number will be closed.

Example:
>
        let g:tagbar_foldlevel = 2
<

                                                          *g:tagbar_iconchars*
g:tagbar_iconchars~

Since the display of the icons used to indicate open or closed folds depends
on the actual font used, different characters may be optimal for different
fonts. With this variable you can set the icons to characters of your liking.
The first character in the list specifies the icon to use for a closed fold,
and the second one for an open fold.

Examples (don't worry if some of the characters aren't displayed correctly,
just choose other characters in that case):
>
        let g:tagbar_iconchars = ['▶', '▼']  (default on Linux and Mac OS X)
        let g:tagbar_iconchars = ['▾', '▸']
        let g:tagbar_iconchars = ['▷', '◢']
        let g:tagbar_iconchars = ['+', '-']  (default on Windows)
<

                                                        *g:tagbar_autoshowtag*
g:tagbar_autoshowtag~
Default: 0

If this variable is set and the current tag is inside of a closed fold then
the folds will be opened as much as needed for the tag to be visible so it can
be highlighted. If it is not set then the folds won't be opened and the parent
tag will be highlighted instead. You can use the |:TagbarShowTag| command to
open the folds manually.

Example:
>
        let g:tagbar_autoshowtag = 1
<

                                              *g:tagbar_updateonsave_maxlines*
g:tagbar_updateonsave_maxlines~
Default: 5000

If the current file has fewer lines than the value of this variable, Tagbar
will update immediately after saving the file. If it is longer then the update
will only happen on the |CursorHold| event and when switching buffers (or
windows). This is to prevent the time it takes to save a large file from
becoming annoying in case you have a slow computer. If you have a fast
computer you can set it to a higher value.

Example:
>
        let g:tagbar_updateonsave_maxlines = 10000
<

                                                          *g:tagbar_systemenc*
g:tagbar_systemenc~
Default: value of 'encoding'

This variable is for cases where the character encoding of your operating
system is different from the one set in Vim, i.e. the 'encoding' option. For
example, if you use a Simplified Chinese Windows version that has a system
encoding of "cp936", and you have set 'encoding' to "utf-8", then you would
have to set this variable to "cp936".

Example:
>
        let g:tagbar_systemenc = 'cp936'
<

------------------------------------------------------------------------------
HIGHLIGHT COLOURS                                           *tagbar-highlight*

All of the colours used by Tagbar can be customized. Here is a list of the
highlight groups that are defined by Tagbar:

TagbarComment
    The help at the top of the buffer.

TagbarKind
    The header of generic "kinds" like "functions" and "variables".

TagbarNestedKind
    The "kind" headers in square brackets inside of scopes.

TagbarScope
    Tags that define a scope like classes, structs etc.

TagbarType
    The type of a tag or scope if available.

TagbarSignature
    Function signatures.

TagbarPseudoID
    The asterisk (*) that signifies a pseudo-tag.

TagbarFoldIcon
    The fold icon on the left of foldable tags.

TagbarHighlight
    The colour that is used for automatically highlighting the current tag.

TagbarAccessPublic
    The "public" visibility/access symbol.

TagbarAccessProtected
    The "protected" visibility/access symbol.

TagbarAccessPrivate
    The "private" visibility/access symbol.

If you want to change any of those colours put a line like the following in
your vimrc:
>
        highlight TagbarScope guifg=Green ctermfg=Green
<
See |:highlight| for more information.

------------------------------------------------------------------------------
AUTOMATICALLY OPENING TAGBAR                                 *tagbar-autoopen*

Since there are several different situations in which you might want to open
Tagbar automatically there is no single option to enable automatic opening.
Instead, autocommands can be used together with a convenience function that
opens Tagbar only if a supported file is open(ed). It has a boolean parameter
that specifies whether Tagbar should be opened if any loaded buffer is
supported (in case the parameter is set to true) or only if a supported
file/buffer is currently being shown in a window. This can be useful if you
use multiple tabs and don't edit supported files in all of them.

If you want to open Tagbar automatically on Vim startup no matter what put
this into your vimrc:
>
        autocmd VimEnter * nested :TagbarOpen
<
If you want to open it only if you're opening Vim with a supported file/files
use this instead:
>
        autocmd VimEnter * nested :call tagbar#autoopen(1)
<
The above is exactly what the Taglist plugin does if you set the
Tlist_Auto_Open option, in case you want to emulate this behaviour.

For opening Tagbar also if you open a supported file in an already running
Vim:
>
        autocmd FileType * nested :call tagbar#autoopen(0)
<
If you use multiple tabs and want Tagbar to also open in the current tab when
you switch to an already loaded, supported buffer:
>
        autocmd BufEnter * nested :call tagbar#autoopen(0)
<
And if you want to open Tagbar only for specific filetypes, not for all of the
supported ones:
>
        autocmd FileType c,cpp nested :TagbarOpen
<
Check out |autocmd.txt| if you want it to open automatically in more
complicated cases.

------------------------------------------------------------------------------
SHOWING THE CURRENT TAG IN THE STATUSLINE                  *tagbar-statusline*

You can show the current tag in the 'statusline', or in any other place that
you want to, by calling the tagbar#currenttag() function. The current tag is
exactly the same as would be highlighted in the Tagbar window if it is open.
It is defined as the nearest tag upwards in the file starting from the cursor
position. This means that for example in a function it should usually be the
name of the function.

The function has the following signature:

tagbar#currenttag({format}, {default} [, {flags}])
    {format} is a |printf()|-compatible format string where "%s" will be
    replaced by the name of the tag. {default} will be displayed instead of
    the format string if no tag can be found.

    The optional {flags} argument specifies some additional properties of the
    displayed tags. It is a string which can contain these character flags:
    'f'   Display the full hierarchy of the tag, not just the tag itself.
    's'   If the tag is a function, the complete signature will be shown,
          otherwise just "()" will be appended to distinguish functions from
          other tags.

    For example, if you put the following into your statusline: >
        %{tagbar#currenttag('[%s] ', '')}
<   then the function "myfunc" will be show as "[myfunc()] ".

==============================================================================
6. Extending Tagbar                                            *tagbar-extend*

Tagbar has a flexible mechanism for extending the existing file type (i.e.
language) definitions. This can be used both to change the settings of the
existing types and to add completely new types. For Tagbar to support a
filetype two things are needed: a program that generates the tag information,
usually Exuberant Ctags, and a Tagbar type definition in your |vimrc| or an
|ftplugin| that tells Tagbar how to interpret the generated tags.

Note: if you only want to customize an existing definition (like changing the
order in which tag kinds are displayed) see "Changing an existing definition"
below.

There are two ways to generate the tag information for new filetypes: add a
definition to Exuberant Ctags or create a specialized program for your
language that generates ctags-compatible tag information (see
|tags-file-format| for information about how a "tags" file is structured). The
former allows simple regular expression-based parsing that is easy to get
started with, but doesn't support scopes unless you instead want to write a
C-based parser module for Exuberant Ctags. The regex approach is described in
more detail below.
Writing your own program is the approach used by for example jsctags and can
be useful if your language can best be parsed by a program written in the
language itself, or if you want to provide the program as part of a complete
support package for the language. Some tips on how to write such a program are
given at the end of this section.

Before writing your own extension have a look at the wiki
(https://github.com/majutsushi/tagbar/wiki/Support-for-additional-filetypes)
or try googling for existing ones. If you do end up creating your own
extension please consider adding it to the wiki so that others will be able to
use it, too.

Every type definition in Tagbar is a dictionary with the following keys:

ctagstype:  The name of the language as recognized by ctags. Use the command >
                ctags --list-languages
<           to get a list of the languages ctags supports. The case doesn't
            matter.
kinds:      A list of the "language kinds" that should be listed in Tagbar,
            ordered by the order they should appear in in the Tagbar window.
            Use the command >
                ctags --list-kinds={language name}
<           to get a list of the kinds ctags supports for a given language. An
            entry in this list is a colon-separated string with the following
            syntax: >
                {short}:{long}[:{fold}[:{stl}]]
<           {short} is the one-character abbreviation that ctags uses, and
            {long} is an arbitrary string that will be used in Tagbar as the
            header for the the tags of this kind that are not listed under a
            specific scope. {fold} determines whether tags of this kind should
            be folded by default, with 1 meaning they should be folded and 0
            they should not. If this part is omitted the tags will not be
            folded by default. {stl} is used by the tagbar#currenttag()
            function (see |tagbar-statusline|) to decide whether tags of this
            kind should be shown in the statusline or not, with 1 meaning they
            will be shown and 0 meaning they will be ignored. Omitting this
            part means that the tags will be shown. Note that you have to
            specify {fold} too if you want to specify {stl}.
            For example, the string >
                "f:functions:1"
<           would list all the function definitions in a file under the header
            "functions", fold them, and implicitly show them in the statusline
            if tagbar#currenttag() is used.
sro:        The scope resolution operator. For example, in C++ it is "::" and
            in Java it is ".". If in doubt run ctags as shown below and check
            the output.
kind2scope: A dictionary describing the mapping of tag kinds (in their
            one-character representation) to the scopes their children will
            appear in, for example classes, structs etc.
            Unfortunately there is no ctags option to list the scopes, you
            have to look at the tags ctags generates manually. For example,
            let's say we have a C++ file "test.cpp" with the following
            contents: >
                class Foo
                {
                public:
                    Foo();
                    ~Foo();
                private:
                    int var;
                };
<           We then run ctags in the following way: >
                ctags -f - --format=2 --excmd=pattern --extra= --fields=nksaSmt test.cpp
<           Then the output for the variable "var" would look like this: >
                var	tmp.cpp /^    int var;$/;"	kind:m	line:11	class:Foo	access:private
<           This shows that the scope name for an entry in a C++ class is
            simply "class". So this would be the word that the "kind"
            character of a class has to be mapped to.
scope2kind: The opposite of the above, mapping scopes to the kinds of their
            parents. Most of the time it is the exact inverse of the above,
            but in some cases it can be different, for example when more than
            one kind maps to the same scope. If it is the exact inverse for
            your language you only need to specify one of the two keys.
replace:    If you set this entry to 1 your definition will completely replace
{optional}  an existing default definition. This is useful if you want to
            disable scopes for a file type for some reason. Note that in this
            case you have to provide all the needed entries yourself!
sort:       This entry can be used to override the global sort setting for
{optional}  this specific file type. The meaning of the value is the same as
            with the global setting, that is if you want to sort tags by name
            set it to 1 and if you want to sort them according to their order
            in the file set it to 0.
deffile:    The path to a file with additional ctags definitions (see the
{optional}  section below on adding a new definition for what exactly that
            means). This is especially useful for ftplugins since they can
            provide a complete type definition with ctags and Tagbar
            configurations without requiring user intervention.
            Let's say you have an ftplugin that adds support for the language
            "mylang", and your directory structure looks like this: >
                ctags/mylang.cnf
                ftplugin/mylang.vim
<           Then the "deffile" entry would look like this to allow for the
            plugin to be installed in an arbitray location (for example
            with pathogen): >

                'deffile' : expand('<sfile>:p:h:h') . '/ctags/mylang.cnf'
<
ctagsbin:  The path to a filetype-specific ctags-compatible program like
{optional} jsctags. Set it in the same way as |g:tagbar_ctags_bin|. jsctags is
           used automatically if found in your $PATH and does not have to be
           set in that case. If it is not in your path you have to set this
           key, the rest of the configuration should not be necessary (unless
           you want to change something, of course). Note: if you use this
           then the "ctagstype" key is not needed.
ctagsargs: The arguments to be passed to the filetype-specific ctags program
{optional} (without the filename). Make sure you set an option that makes the
           program output its data on stdout. Not used for the normal ctags
           program.


You then have to assign this dictionary to a variable in your vimrc with the
name
>
        g:tagbar_type_{vim filetype}
<
For example, for C++ the name would be "g:tagbar_type_cpp". If you don't know
the vim file type then run the following command:
>
        :set filetype?
<
and vim will display the file type of the current buffer.

Example: C++~
Here is a complete example that shows the default configuration for C++ as
used in Tagbar. This is just for illustration purposes since user
configurations will usually be less complicated.
>
        let g:tagbar_type_cpp = {
            \ 'ctagstype' : 'c++',
            \ 'kinds'     : [
                \ 'd:macros:1:0',
                \ 'p:prototypes:1:0',
                \ 'g:enums',
                \ 'e:enumerators:0:0',
                \ 't:typedefs:0:0',
                \ 'n:namespaces',
                \ 'c:classes',
                \ 's:structs',
                \ 'u:unions',
                \ 'f:functions',
                \ 'm:members:0:0',
                \ 'v:variables:0:0'
            \ ],
            \ 'sro'        : '::',
            \ 'kind2scope' : {
                \ 'g' : 'enum',
                \ 'n' : 'namespace',
                \ 'c' : 'class',
                \ 's' : 'struct',
                \ 'u' : 'union'
            \ },
            \ 'scope2kind' : {
                \ 'enum'      : 'g',
                \ 'namespace' : 'n',
                \ 'class'     : 'c',
                \ 'struct'    : 's',
                \ 'union'     : 'u'
            \ }
        \ }
<

Which of the keys you have to specify depends on what you want to do.

Changing an existing definition~
If you want to change an existing definition you only need to specify the
parts that you want to change. It probably only makes sense to change "kinds",
which would be the case if you wanted to for example change the order of
certain kinds, change their default fold state or exclude them from appearing
in Tagbar. The easiest way to do that is to use the |:TagbarGetTypeConfig|
command, which will paste a ready-to-use configuration with the "kinds" entry
for the specified type at the current cursor position.

As an example, if you didn't want Tagbar to show prototypes for C++ files,
switch the order of enums and typedefs, and show macros in the statusline, you
would first run ":TagbarGetTypeConfig cpp" in your vimrc and then change the
definition like this:
>
        let g:tagbar_type_cpp = {
            \ 'kinds' : [
                \ 'd:macros:1',
                \ 'g:enums',
                \ 't:typedefs:0:0',
                \ 'e:enumerators:0:0',
                \ 'n:namespaces',
                \ 'c:classes',
                \ 's:structs',
                \ 'u:unions',
                \ 'f:functions',
                \ 'm:members:0:0',
                \ 'v:variables:0:0'
            \ ]
        \ }
<
Compare with the complete example above to see the difference.

Adding a definition for a new language/file type~
In order to be able to add a new language to Tagbar you first have to create a
configuration for ctags that it can use to parse the files. This can be done
in two ways:

  1. Use the --regex argument for specifying regular expressions that are used
     to parse the files. An example of this is given below. A disadvantage of
     this approach is that you can't specify scopes.
  2. Write a parser plugin in C for ctags. This approach is much more powerful
     than the regex approach since you can make use of all of ctags'
     functionality but it also requires much more work. Read the ctags
     documentation for more information about how to do this.

For the first approach the only keys that are needed in the Tagbar definition
are "ctagstype" and "kinds". A definition that supports scopes has to define
those two and in addition "scopes", "sro" and at least one of "kind2scope" and
"scope2kind".

Let's assume we want to add support for LaTeX to Tagbar using the regex
approach. First we put the following text into ~/.ctags or a file pointed to
by the "deffile" definition entry:
>
        --langdef=latex
        --langmap=latex:.tex
        --regex-latex=/^\\tableofcontents/TABLE OF CONTENTS/s,toc/
        --regex-latex=/^\\frontmatter/FRONTMATTER/s,frontmatter/
        --regex-latex=/^\\mainmatter/MAINMATTER/s,mainmatter/
        --regex-latex=/^\\backmatter/BACKMATTER/s,backmatter/
        --regex-latex=/^\\bibliography\{/BIBLIOGRAPHY/s,bibliography/
        --regex-latex=/^\\part[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/PART \2/s,part/
        --regex-latex=/^\\part[[:space:]]*\*[[:space:]]*\{([^}]+)\}/PART \1/s,part/
        --regex-latex=/^\\chapter[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/CHAP \2/s,chapter/
        --regex-latex=/^\\chapter[[:space:]]*\*[[:space:]]*\{([^}]+)\}/CHAP \1/s,chapter/
        --regex-latex=/^\\section[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/\. \2/s,section/
        --regex-latex=/^\\section[[:space:]]*\*[[:space:]]*\{([^}]+)\}/\. \1/s,section/
        --regex-latex=/^\\subsection[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/\.\. \2/s,subsection/
        --regex-latex=/^\\subsection[[:space:]]*\*[[:space:]]*\{([^}]+)\}/\.\. \1/s,subsection/
        --regex-latex=/^\\subsubsection[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/\.\.\. \2/s,subsubsection/
        --regex-latex=/^\\subsubsection[[:space:]]*\*[[:space:]]*\{([^}]+)\}/\.\.\. \1/s,subsubsection/
        --regex-latex=/^\\includegraphics[[:space:]]*(\[[^]]*\])?[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/\3/g,graphic+listing/
        --regex-latex=/^\\lstinputlisting[[:space:]]*(\[[^]]*\])?[[:space:]]*(\[[^]]*\])?[[:space:]]*\{([^}]+)\}/\3/g,graphic+listing/
        --regex-latex=/\\label[[:space:]]*\{([^}]+)\}/\1/l,label/
        --regex-latex=/\\ref[[:space:]]*\{([^}]+)\}/\1/r,ref/
        --regex-latex=/\\pageref[[:space:]]*\{([^}]+)\}/\1/p,pageref/
<
This will create a new language definition with the name "latex" and associate
it with files with the extension ".tex". It will also define the kinds "s" for
sections, chapters and the like, "g" for included graphics, "l" for labels,
"r" for references and "p" for page references. See the ctags documentation
for more information about the exact syntax.

Now we have to create the Tagbar language definition in our vimrc:
>
        let g:tagbar_type_tex = {
            \ 'ctagstype' : 'latex',
            \ 'kinds'     : [
                \ 's:sections',
                \ 'g:graphics:0:0',
                \ 'l:labels',
                \ 'r:refs:1:0',
                \ 'p:pagerefs:1:0'
            \ ],
            \ 'sort'    : 0,
            \ 'deffile' : expand('<sfile>:p:h:h') . '/ctags/latex.cnf'
        \ }
<
The "deffile" field is of course only needed if the ctags definition actually
is in that file and not in ~/.ctags.

Sort has been disabled for LaTeX so that the sections appear in their correct
order. They unfortunately can't be shown nested with their correct scopes
since as already mentioned the regular expression approach doesn't support
that.

Tagbar should now be able to show the sections and other tags from LaTeX
files.

Writing your own tag-generating program~
If you want to write your own program for generating tags then here are some
imporant tips to get it to integrate well with Tagbar:

  - Tagbar supports the same tag format as Vim itself. The format is described
    in |tags-file-format|, the third format mentioned there is the relevant
    one. Note that the {tagaddress} part should be a search pattern since the
    line number can be specified in a field (see below).
  - Tagbar reads the tag information from a program's standard output
    (stdout), it doesn't generate files and reads them in after that. So make
    sure that your program has an option to output the tags on stdout.
  - Some fields are supported for providing additional information about a
    tag. One field is required: the "kind" field as a single letter without
    a "kind:" fieldname. This field has to be the first one in the list. All
    other fields need to have a fieldname in order to determine what they are.
    The following fields are supported for all filetypes:

        * line:      The line number of the tag
        * column:    The column number of the tag
        * signature: The signature of a function
        * access:    Visibility/access information of a tag; the values
                     "public", "protected" and "private" will be denoted with
                     a special symbol in Tagbar

    In addition fields that describe the surrounding scope of the tag are
    supported if they are specified in the type configuration as explained at
    the beginning of this section. For example, for a tag in class "Foo" this
    could look like "class:Foo".

==============================================================================
7. Troubleshooting & Known issues                              *tagbar-issues*

As a general rule, if the tag information displayed by Tagbar is wrong (for
example, a method doesn't show up or is in the wrong place) you should first
try running ctags manually to see whether ctags reports the wrong information
or whether that information is correct and Tagbar does something wrong. To run
ctags manually execute the following command in a terminal:
>
        ctags -f - --format=2 --excmd=pattern --extra= --fields=nksaSmt myfile
<
If you set the |g:tagbar_ctags_bin| variable you probably have to use the same
value here instead of simply "ctags".

If Tagbar doesn't seem to work at all, but you don't get any error messages,
you can use Tagbar's debug mode to try to find the source of the problem (see
|tagbar-commands| on how to invoke it). In that case you should especially pay
attention to the reported file type and the ctags command line in the log
file.


  - jsctags has to be newer than 2011-01-06 since it needs the "-f" option to
    work. Also, the output of jsctags seems to be a bit unreliable at the
    moment (especially regarding line numbers), so if you notice some strange
    behaviour with it please run it manually in a terminal to check whether
    the bug is in jsctags or Tagbar.

  - Nested pseudo-tags cannot be properly parsed since only the direct parent
    scope of a tag gets assigned a type, the type of the grandparents is not
    reported by ctags (assuming the grandparents don't have direct, real
    children).

    For example, if we have a C++ file with the following content:
>
        foo::Bar::init()
        {
            // ...
        }
        foo::Baz::method()
        {
            // ...
        }
<
    In this case the type of "foo" is not known. Is it a namespace? A class?
    For this reason the methods are displayed in Tagbar like this:
>
        foo::Bar* : class
          init()
        foo::Baz* : class
          method()
<
  - Scope-defining tags at the top level that have the same name but a
    different kind/scope type can lead to an incorrect display. For example,
    the following Python code will incorrectly insert a pseudo-tag "Inner2"
    into the "test" class:
>
        class test:
            class Inner:
                def foo(self):
                    pass

        def test():
            class Inner2:
                def bar(self):
                    pass
<
    I haven't found a proper way around this yet, but it shouldn't be much of
    a problem in practice anyway. Tags with the same name at any other level
    are no problem, though.

==============================================================================
8. History                                                    *tagbar-history*

2.4 (2012-06-17)
    - New function tagbar#currenttag() that reports the current tag, for
      example for putting it into the statusline.
    - New command TagbarGetTypeConfig for easy customization of an existing
      type.
    - Type definitions now can be loaded from ftplugins.
    - The autoopen() function is now a bit more flexible.
    - Vala is now supported if Anjuta is installed.
    - Various other small improvements and bugfixes.

2.3 (2011-12-24)
    - Add a convenience function that allows more flexible ways to
      automatically open Tagbar.
    - Replace option tagbar_usearrows with tagbar_iconchars to allow custom
      characters to be specified. This helps with fonts that don't display the
      default characters properly.
    - Remove the need to provide the complete jsctags configuration if jsctags
      is not found in $PATH, now only the concrete path has to be specified.
    - Add debugging functionality.

2.2 (2011-11-26)
    - Small incompatible change: TagbarOpen now doesn't jump to the Tagbar
      window anymore if it is already open. Use "TagbarOpen j" instead or see
      its documentation for more options.
    - Tags inside of scopes now have a header displaying their "kind".
    - The Tagbar contents are now immediately updated on save for files
      smaller than a configurable size.
    - Tagbar can now be configured to jump to a tag with only a single-click
      instead of a double-click.
    - Most of the script has been moved to the |autoload| directory, so Vim
      startup should be faster (thanks to Kien N).
    - Jumping to tags should work most of the time even if the file has been
      modified and not saved.
    - If Ctags has been installed into the default location using Homebrew or
      MacPorts it should now be found automatically.
    - Several bugfixes.

2.1 (2011-05-29)
    - Make Tagbar work in (hopefully) all cases under Windows
    - Handle cases where 'encoding' is different from system encoding, for
      example on a Chinese Windows with 'encoding' set to "utf-8" (see manual
      for details in case it doesn't work out-of-the-box)
    - Fixed a bug with the handling of subtypes like "python.django"
    - If a session got saved with Tagbar open it now gets restored properly
    - Locally reset foldmethod/foldexpr in case foldexpr got set to something
      expensive globally
    - Tagbar now tries hard to go to the correct window when jumping to a tag
    - Explain some possible issues with the current jsctags version in the
      manual
    - Explicitly check for some possible configuration problems to be able to
      give better feedback
    - A few other small fixes

2.0.1 (2011-04-26)
    - Fix sorting bug when 'ignorecase' is set

2.0 (2011-04-26)
    - Folding now works correctly. Folds will be preserved when leaving the
      Tagbar window and when switching between files. Also tag types can be
      configured to be folded by default, which is useful for things like
      includes and imports.
    - DoctorJS/jsctags and other compatible programs are now supported.
    - All of the highlight groups can now be overridden.
    - Added keybinding to quickly jump to next/previous top-level tag.
    - Added Taglist's "p" keybinding for jumping to a tag without leaving the
      Tagbar window.
    - Several bugfixes and other small improvements.

1.5 (2011-03-06)
    - Type definitions can now include a path to a file with the ctags
      definition. This is especially useful for ftplugins that can now ship
      with a complete ctags and Tagbar configuration without requiring user
      intervention. Thanks to Jan Christoph Ebersbach for the suggestion.
    - Added autofocus setting by Taybin Rutkin. This will put the cursor in
      the Tagbar window when it is opened.
    - The "scopes" field is no longer needed in type definitions, the
      information is already there in "scope2kind". Existing definitions will
      be ignored.
    - Some fixes and improvements related to redrawing and window switching.

1.2 (2011-02-28)
    - Fix typo in Ruby definition

1.1 (2011-02-26)
    - Don't lose syntax highlighting when ':syntax enable' is called
    - Allow expanding the Vim window when Tagbar is opened

1.0 (2011-02-23)
    - Initial release

==============================================================================
9. Todo                                                          *tagbar-todo*

  - Allow filtering the Tagbar content by some criteria like tag name,
    visibility, kind ...
  - Integrate Tagbar with the FSwitch plugin to provide header file
    information in C/C++.
  - Allow jumping to a tag in the preview window, a split window or a new tab.

==============================================================================
10. Credits                                                   *tagbar-credits*

Tagbar was written by Jan Larres and is released under the Vim licence, see
|license|. It was heavily inspired by the Taglist plugin by Yegappan
Lakshmanan and uses a small amount of code from it.

Original taglist copyright notice:
Permission is hereby granted to use and distribute this code, with or without
modifications, provided that this copyright notice is copied with it. Like
anything else that's free, taglist.vim is provided *as is* and comes with no
warranty of any kind, either expressed or implied. In no event will the
copyright holder be liable for any damamges resulting from the use of this
software.

The folding technique was inspired by NERDTree by Martin Grenfell.

Thanks to the following people for code contributions, feature suggestions etc:
Jan Christoph Ebersbach
Vadim Fint
Leandro Freitas
Seth Milliken
Kien N
pielgrzym
Taybin Rutkin
Ville Valkonen

==============================================================================
 vim: tw=78 ts=8 sw=4 sts=4 et ft=help
plugin/tagbar.vim	[[[1
119
" ============================================================================
" File:        tagbar.vim
" Description: List the current file's tags in a sidebar, ordered by class etc
" Author:      Jan Larres <jan@majutsushi.net>
" Licence:     Vim licence
" Website:     http://majutsushi.github.com/tagbar/
" Version:     2.4
" Note:        This plugin was heavily inspired by the 'Taglist' plugin by
"              Yegappan Lakshmanan and uses a small amount of code from it.
"
" Original taglist copyright notice:
"              Permission is hereby granted to use and distribute this code,
"              with or without modifications, provided that this copyright
"              notice is copied with it. Like anything else that's free,
"              taglist.vim is provided *as is* and comes with no warranty of
"              any kind, either expressed or implied. In no event will the
"              copyright holder be liable for any damamges resulting from the
"              use of this software.
" ============================================================================

scriptencoding utf-8

if &cp || exists('g:loaded_tagbar')
    finish
endif

" Basic init {{{1

if v:version < 700
    echohl WarningMsg
    echomsg 'Tagbar: Vim version is too old, Tagbar requires at least 7.0'
    echohl None
    finish
endif

if v:version == 700 && !has('patch167')
    echohl WarningMsg
    echomsg 'Tagbar: Vim versions lower than 7.0.167 have a bug'
          \ 'that prevents this version of Tagbar from working.'
          \ 'Please use the alternate version posted on the website.'
    echohl None
    finish
endif

if !exists('g:tagbar_left')
    let g:tagbar_left = 0
endif

if !exists('g:tagbar_width')
    let g:tagbar_width = 40
endif

if !exists('g:tagbar_autoclose')
    let g:tagbar_autoclose = 0
endif

if !exists('g:tagbar_autofocus')
    let g:tagbar_autofocus = 0
endif

if !exists('g:tagbar_sort')
    let g:tagbar_sort = 1
endif

if !exists('g:tagbar_compact')
    let g:tagbar_compact = 0
endif

if !exists('g:tagbar_expand')
    let g:tagbar_expand = 0
endif

if !exists('g:tagbar_singleclick')
    let g:tagbar_singleclick = 0
endif

if !exists('g:tagbar_foldlevel')
    let g:tagbar_foldlevel = 99
endif

if !exists('g:tagbar_iconchars')
    if has('multi_byte') && has('unix') && &encoding == 'utf-8' &&
     \ (empty(&termencoding) || &termencoding == 'utf-8')
        let g:tagbar_iconchars = ['▶', '▼']
    else
        let g:tagbar_iconchars = ['+', '-']
    endif
endif

if !exists('g:tagbar_autoshowtag')
    let g:tagbar_autoshowtag = 0
endif

if !exists('g:tagbar_updateonsave_maxlines')
    let g:tagbar_updateonsave_maxlines = 5000
endif

if !exists('g:tagbar_systemenc')
    let g:tagbar_systemenc = &encoding
endif

augroup TagbarSession
    autocmd!
    autocmd SessionLoadPost * nested call tagbar#RestoreSession()
augroup END

" Commands {{{1
command! -nargs=0 TagbarToggle        call tagbar#ToggleWindow()
command! -nargs=? TagbarOpen          call tagbar#OpenWindow(<f-args>)
command! -nargs=0 TagbarOpenAutoClose call tagbar#OpenWindow('fc')
command! -nargs=0 TagbarClose         call tagbar#CloseWindow()
command! -nargs=1 -bang TagbarSetFoldlevel  call tagbar#SetFoldLevel(<args>, <bang>0)
command! -nargs=0 TagbarShowTag       call tagbar#OpenParents()
command! -nargs=1 TagbarGetTypeConfig call tagbar#gettypeconfig(<f-args>)
command! -nargs=? TagbarDebug         call tagbar#StartDebug(<f-args>)
command! -nargs=0 TagbarDebugEnd      call tagbar#StopDebug()

" Modeline {{{1
" vim: ts=8 sw=4 sts=4 et foldenable foldmethod=marker foldcolumn=1
syntax/tagbar.vim	[[[1
63
" File:        tagbar.vim
" Description: Tagbar syntax settings
" Author:      Jan Larres <jan@majutsushi.net>
" Licence:     Vim licence
" Website:     http://majutsushi.github.com/tagbar/
" Version:     2.4

scriptencoding utf-8

if exists("b:current_syntax")
  finish
endif

let s:ic = g:tagbar_iconchars[0]
if s:ic =~ '[]^\\-]'
    let s:ic = '\' . s:ic
endif
let s:io = g:tagbar_iconchars[1]
if s:io =~ '[]^\\-]'
    let s:io = '\' . s:io
endif

let s:pattern = '\([' . s:ic . s:io . '] \)\@<=[^-+: ]\+[^:]\+$'
execute "syntax match TagbarKind '" . s:pattern . "'"

let s:pattern = '\([' . s:ic . s:io . '][-+# ]\)\@<=[^*]\+\(\*\?\(([^)]\+)\)\? :\)\@='
execute "syntax match TagbarScope '" . s:pattern . "'"

let s:pattern = '[' . s:ic . s:io . ']\([-+# ]\)\@='
execute "syntax match TagbarFoldIcon '" . s:pattern . "'"

let s:pattern = '\([' . s:ic . s:io . ' ]\)\@<=+\([^-+# ]\)\@='
execute "syntax match TagbarAccessPublic '" . s:pattern . "'"
let s:pattern = '\([' . s:ic . s:io . ' ]\)\@<=#\([^-+# ]\)\@='
execute "syntax match TagbarAccessProtected '" . s:pattern . "'"
let s:pattern = '\([' . s:ic . s:io . ' ]\)\@<=-\([^-+# ]\)\@='
execute "syntax match TagbarAccessPrivate '" . s:pattern . "'"

unlet s:pattern

syntax match TagbarNestedKind '^\s\+\[[^]]\+\]$'
syntax match TagbarComment    '^".*'
syntax match TagbarType       ' : \zs.*'
syntax match TagbarSignature  '(.*)'
syntax match TagbarPseudoID   '\*\ze :'

highlight default link TagbarComment    Comment
highlight default link TagbarKind       Identifier
highlight default link TagbarNestedKind TagbarKind
highlight default link TagbarScope      Title
highlight default link TagbarType       Type
highlight default link TagbarSignature  SpecialKey
highlight default link TagbarPseudoID   NonText
highlight default link TagbarFoldIcon   Statement
highlight default link TagbarHighlight  Search

highlight default TagbarAccessPublic    guifg=Green ctermfg=Green
highlight default TagbarAccessProtected guifg=Blue  ctermfg=Blue
highlight default TagbarAccessPrivate   guifg=Red   ctermfg=Red

let b:current_syntax = "tagbar"

" vim: ts=8 sw=4 sts=4 et foldenable foldmethod=marker foldcolumn=1
