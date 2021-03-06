-- Return a table containing meta data that describes the package contents.
return {
  title = "Tutorial",
  author = "O|D",
  name = "tutorial",
  units = {
    {
      -- Title and category control how this unit appears in the unit browser.
      title = "FM Operator",
      category = "Learning",
      -- Which lua module contains the unit definition?
      moduleName = "FMOperator",
    },
  }
}

-- All packages must have this file!