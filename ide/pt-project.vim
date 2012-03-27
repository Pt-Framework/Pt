
let s:project_file = expand("<sfile>")

let s:project_dir = strpart(s:project_file, 0, match(s:project_file, "pt-project.vim"))
exec "cd " . s:project_dir
exec "cd .."
let g:build_root = getcwd()

let &path = g:build_root . "/include," . g:build_root . "/src/Pt," . g:build_root . "/src/Pt-System," . ","

:silent! so Session.vim

if filereadable(g:build_root . "/pt.tags")
    let &tags = g:build_root . "/pt.tags"
else
    let s:confirmed = confirm("Do you want to generate tags?", "&Yes\n&No", 2)
    if s:confirmed == 1
        :call system("ctags -f pt.tags --c-kinds=+p --fields=+iaS --extra=+q --language-force=C++ -IPT_API include/Pt/*.h src/Pt/*.cpp")
        :call system("ctags --append=on -f pt.tags --c-kinds=+p --fields=+iaS --extra=+q --language-force=C++ -IPT_SYSTEM_API include/Pt/System/*.h src/Pt-System/*.cpp")
        let &tags = g:build_root . "/pt.tags" 
    endif
endif

"
" Build command
"
if has('win32')
    let s:makeprg_line="jam.bat -q $*"
    let &makeprg=s:makeprg_line
    let g:make_target=''
    let g:build_root=''
else
    let s:makeprg_line="./jam.sh -q $*"
    let &makeprg=s:makeprg_line
    let g:make_target=''
    let g:build_root=''
endif

" See also (very nice!!!) http://blog.vicshih.com/2011/03/fast-make-for-vim.html
function BuildMake(make_args)
  " close QuickFix window (content is now outdated due to new build run)
  cclose

  " Force write
  sil write!

  " http://vim.1045645.n5.nabble.com/Working-directory-for-make-td1172840.html
  " http://stackoverflow.com/questions/1043432/omit-pattern-not-found-error-message-in-vim-script
  let cmd_cd='sil lcd ' . g:build_root
  exe cmd_cd
  
  " pwd
  " need to use "exe" to get variables evaluated properly:
  " http://stackoverflow.com/questions/4596932/vim-cd-to-path-stored-in-variable
  " and we should _not_ run exe on the raw line directly, since this will execute _output_
  " results given by make!
  let cmd_make="make" . a:make_args
  exe cmd_make
  
  " open quickfix win if there are errors
  botright copen

  "copen
  "cc 1 " this does not appear to be necessary since opening the window will jump to first issue anyway
  sil lcd -
endfunction

:noremap <F5> :call BuildMake(g:make_target)<CR>
:noremap! <F5> <C-o>:call BuildMake(g:make_target)<CR>

