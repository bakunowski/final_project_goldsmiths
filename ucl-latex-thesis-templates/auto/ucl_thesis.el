(TeX-add-style-hook
 "ucl_thesis"
 (lambda ()
   (TeX-run-style-hooks
    "latex2e"
    "kcl_thesis"
    "kcl_thesis10"
    "book"
    "bk10"
    "times"
    "mathptmx")
   (TeX-add-symbols
    "department")
   (LaTeX-add-environments
    "abstract"
    "acknowledgements"
    "impactstatement")
   (LaTeX-add-pagestyles
    "headings"))
 :latex)

