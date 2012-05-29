""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" Set project directory
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
exec "cd " . expand("<sfile>:h")
exec "cd .."
let g:pt_project_root = getcwd()

""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" Load last session
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
:silent! so Session.vim

:nmap _g :grep <C-R>=expand("<cword>")<CR><CR>

""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" C-Tags support
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
let s:ctags_file   = "pt.tags"
let s:ctags_create = "ctags -f " . s:ctags_file . " --c-kinds=+p --fields=+iaS --extra=+q --language-force=C++ "  
let s:ctags_append = "ctags --append=on -f " . s:ctags_file . " --c-kinds=+p --fields=+iaS --extra=+q --language-force=C++ "
" ctags -R --sort=1 --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++ -IPT_API -f pt.tags src include
function TagsBuild()
    :call system(s:ctags_create . "-IPT_API include/Pt/*.h src/Pt/*.cpp")
    :call system(s:ctags_append . "-IPT_SYSTEM_API include/Pt/System/*.h src/Pt-System/*.cpp")
    :call system(s:ctags_append . "-IPT_NET_API include/Pt/Net/*.h src/Pt-Net/*.cpp")
    :call system(s:ctags_append . "-IPT_UNIT_API include/Pt/Unit/*.h src/Pt-Unit/*.cpp")
    :call system(s:ctags_append . "-IPT_SSL_API include/Pt/Ssl/*.h src/Pt-Ssl/*.cpp")
    :call system(s:ctags_append . "-IPT_HTTP_API include/Pt/Http/*.h src/Pt-Http/*.cpp")
    :call system(s:ctags_append . "-IPT_XML_API include/Pt/Xml/*.h src/Pt-Xml/*.cpp")
    :call system(s:ctags_append . "-IPT_XMLRPC_API include/Pt/XmlRpc/*.h src/Pt-XmlRpc/*.cpp")
    let &tags = g:pt_project_root . "/" . s:ctags_file
endfunction

if ! filereadable(g:pt_project_root . "/" . s:ctags_file)
    let s:confirmed = confirm("Do you want to generate tags?", "&Yes\n&No", 2)
    if s:confirmed == 1
        :call TagsBuild()
    endif
else
    let &tags = g:pt_project_root . "/" . s:ctags_file
endif

command! PtTagsBuild :call TagsBuild()

""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" Jam build support
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
let g:jam_args = ''

if has('win32')
    let &makeprg = "jam.bat -q $*"
    let &shellpipe = "2>&1| ide\\tee.exe"
else
    let &makeprg = "./jam.sh -q $*"
endif

function Build(args)
    " See also (very nice!!!) http://blog.vicshih.com/2011/03/fast-make-for-vim.html
    " close QuickFix window (content is now outdated due to new build run)
    cclose
    sil wall!
    exe "sil lcd " . g:pt_project_root
    exe "make " . a:args 
    botright copen
    "cc 1 "j umps to first error
    sil lcd -
endfunction

:noremap <F5> :call Build(g:jam_args)<CR>
:noremap! <F5> <C-o>:call Build(g:jam_args)<CR>

command! PtBuild :call Build('')

