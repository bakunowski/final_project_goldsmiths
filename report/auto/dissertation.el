(TeX-add-style-hook
 "dissertation"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("report" "a4paper" "draft")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("textpos" "absolute") ("geometry" "left=1.2in" "right=1.2in")))
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperref")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperimage")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperbaseurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "nolinkurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "url")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "path")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "path")
   (TeX-run-style-hooks
    "latex2e"
    "abstract"
    "acknowledgements"
    "license"
    "abbreviations"
    "intro"
    "literature"
    "methods"
    "results_analysis"
    "discussion"
    "conclusion"
    "report"
    "rep10"
    "graphicx"
    "amsmath"
    "amsfonts"
    "amssymb"
    "natbib"
    "rotating"
    "hyperref"
    "subfig"
    "appendix"
    "tipa"
    "clrscode"
    "setspace"
    "textpos"
    "geometry")
   (TeX-add-symbols
    '("OOm" 1)
    '("OT" 1))
   (LaTeX-add-bibliographies
    "refs"))
 :latex)

