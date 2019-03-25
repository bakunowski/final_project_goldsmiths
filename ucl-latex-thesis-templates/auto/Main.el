(TeX-add-style-hook
 "Main"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("ucl_thesis" "11pt" "phd" "a4paper" "oneside")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("nag" "l2tabu" "orthodox")))
   (TeX-run-style-hooks
    "latex2e"
    "MainPackages"
    "LinksAndMetadata"
    "FloatSettings"
    "BibSettings"
    "Preamble"
    "Introduction"
    "Chapter2"
    "Chapter3"
    "Chapter4"
    "Chapter5"
    "Conclusions"
    "Appendices"
    "nag"
    "ucl_thesis"
    "ucl_thesis11")
   (LaTeX-add-bibliographies
    "example"))
 :latex)

