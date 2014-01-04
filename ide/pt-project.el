;;
;; Jam Menu
;;
(define-key-after global-map [menu-bar build-menu]
  (cons "Jam" (make-sparse-keymap "Jam")) 'project-menu )

(defvar jam-config-debug t)

;; (define-key-after (lookup-key global-map [menu-bar build-menu]) [debug-toggle]
;;   (menu-bar-make-toggle toggle-jam-debug jam-config-debug
;; 	    "Debug" "Debug builds toggled" "Builds debug versions"))

(defvar menu-bar-config-menu (make-sparse-keymap "Build Config"))

(define-key menu-bar-config-menu [menu-bar-config-debug]
  '(menu-item "Debug"
	      (lambda ()
		(interactive)
        (setq jam-config-debug t)
		(message "Using debug configuration"))
	      :help "Build debug version"
	      :button (:radio . (eq jam-config-debug t))
	      :visible (menu-bar-menu-frame-live-and-visible-p)))

(define-key menu-bar-config-menu [menu-bar-config-release]
  '(menu-item "Release"
	      (lambda ()
		(interactive)
		(setq jam-config-debug nil)
		(message "Using release configuration"))
	      :help "Build release version"
	      :button (:radio . (not jam-config-debug))
	      :visible (menu-bar-menu-frame-live-and-visible-p)))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [config-menu]
  (list 'menu-item "Configuration" menu-bar-config-menu))

(define-key-after (lookup-key global-map [menu-bar build-menu])
  [build-menu-separator0] '("--" . build-menu-separator0) t)

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build1-label]
  '(menu-item "Build Runtime"
     (lambda () (interactive)
       (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug Pt Pt-System Pt-Net Pt-Xml Pt-XmlRpc")
           (compile "./jam.sh -q -sCONFIG=release Pt Pt-System Pt-Net Pt-Xml Pt-XmlRpc")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build2-label]
  '(menu-item "Build All"
     (lambda () (interactive) (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug")
           (compile "./jam.sh -q -sCONFIG=release")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build4-label]
  '(menu-item "Clean All"
     (lambda () (interactive)
       (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug clean")
           (compile "./jam.sh -q -sCONFIG=release clean")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu])
  [build-menu-separator1] '("--" . build-menu-separator1) t)

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build3-label]
  '(menu-item "Test All"
     (lambda () (interactive) (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug test")
           (compile "./jam.sh -q -sCONFIG=release test")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build8-label]
  '(menu-item "Test Pt"
     (lambda () (interactive) (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug Pt-test.unit")
           (compile "./jam.sh -q -sCONFIG=release Pt-test.unit")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build9-label]
  '(menu-item "Test Pt-System"
     (lambda () (interactive) (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug Pt-System-test.unit")
           (compile "./jam.sh -q -sCONFIG=release Pt-System-test.unit")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build10-label]
  '(menu-item "Test Pt-Xml"
     (lambda () (interactive) (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug Pt-Xml-test.unit")
           (compile "./jam.sh -q -sCONFIG=release Pt-Xml-test.unit")))
     nil))

(define-key-after (lookup-key global-map [menu-bar build-menu]) [build11-label]
  '(menu-item "Test Pt-XmlRpc"
     (lambda () (interactive) (cd project-directory)
       (if jam-config-debug
           (compile "./jam.sh -q -sCONFIG=debug Pt-XmlRpc-test.unit")
           (compile "./jam.sh -q -sCONFIG=release Pt-XmlRpc-test.unit")))
     nil))

(add-hook 'project-close-hook
  (lambda ()
    (define-key global-map [menu-bar build-menu] 'undefined)
    (message "Closed project.")))

