#set page(
  paper: "a4",
  margin: (x: 2.5cm, top: 3cm, bottom: 2.5cm),
  header: align(right, text(fill: gray, size: 9pt)[Modélisation Mathématique — ESP32 System]),
  footer: [
    #set text(fill: gray, size: 9pt)
    #grid(
      columns: (1fr, 1fr),
      [Statut: Version Initiale],
      //align(right, counter(page).display())
    )
  ]
)
#set text(
  font: "Liberation Sans",
  size: 11pt,
  lang: "fr"
)
#set par(justify: true)

// Style des titres
#show heading: it => [
  #set text(fill: rgb("1c3d5a"))
  #v(0.5em)
  #it
  #v(0.2em)
]

// Page de garde / Titre principal
#align(center)[
  #v(2cm)
  #text(size: 26pt, weight: "bold", fill: rgb("1c3d5a"))[Specifiaction and Modelisation]
  
  
  #v(1cm)
  *Auteur:* Luis Brunard \
  *Date:* #datetime.today().display("[day]/[month]/[year]")
]

#v(2cm)
#line(length: 100%, stroke: 0.5pt + gray)

= 1. Introduction
Ce document présente la modélisation mathématique formelle des contraintes de notre système embarqué. L'objectif est de valider par le calcul l'autonomie énergétique, l'allocation mémoire et le déterminisme temporel face aux contraintes physiques.

= 2. System specifications
#align(center)[
  #grid(
    columns : 2,
    column-gutter: 100pt,
    align: center, 
    row-gutter: 10pt,
    [Inputs], [Output],
    table(
      columns: 2,
      [Name], [Button], 
      [`bp`], [Volume +],
      [`bm`], [Volume -],
      [`bn`], [Next], 
      [`bv`], [Previous],
      [`bs`], [Source],
      [`ok`], [recieved in less then 300ms],
    ),
    
    table(
      columns: 2,
      [Name], [Corresponding Opto], 
      [`op`], [Volume +],
      [`om`], [Volume -],
      [`on`], [Next], 
      [`ov`], [Previous],
      [`os`], [Source],
    )
  )
]

==



