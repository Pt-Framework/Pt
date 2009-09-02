;;
;; Jam Menu
;;
(define-key-after global-map [menu-bar build-menu]
  (cons "Jam" (make-sparse-keymap "Jam"))
  'project-menu )

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build1-label]
  '(menu-item "Build Runtime (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -q Pt Pt-System Pt-Net Pt-Xml Pt-XmlRpc") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build2-label]
  '(menu-item "Build All (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -sCONFIG=debug -q") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build3-label]
  '(menu-item "Test All (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -sCONFIG=debug -q test") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build4-label]
  '(menu-item "Clean All (debug)"
     (lambda () (interactive)
       (cd project-directory)
       (compile "./jam.sh -sCONFIG=debug -q clean")
     nil)))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build5-label]
  '(menu-item "Build All (release)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -sCONFIG=release -q") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build6-label]
  '(menu-item "Test All (release)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -sCONFIG=release -q test") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build7-label]
  '(menu-item "Clean All (release)"
     (lambda () (interactive)
       (cd project-directory)
       (compile "./jam.sh -sCONFIG=release -q clean")
     nil)))

(define-key-after (lookup-key global-map [menu-bar build-menu])
  [build-menu-separator0] '("--" . build-menu-separator0) t)

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build8-label]
  '(menu-item "Test Pt (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -q Pt-test.unit") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build9-label]
  '(menu-item "Test Pt-System (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -q Pt-System-test.unit") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build10-label]
  '(menu-item "Test Pt-Xml (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -q Pt-Xml-test.unit") )
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build11-label]
  '(menu-item "Test Pt-XmlRpc (debug)"
     (lambda () (interactive) (cd project-directory) (compile "./jam.sh -q Pt-XmlRpc-test.unit") )
     nil))


