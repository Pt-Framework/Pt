
let s:project_file = expand("<sfile>")

let s:project_dir = strpart(s:project_file, 0, match(s:project_file, "pt-project.vim"))
exec "cd " . s:project_dir
exec "cd .."
let g:build_root = getcwd()

