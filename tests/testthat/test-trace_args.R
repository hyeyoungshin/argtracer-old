test_that("trace args", {
  r <- trace_args(code = {
    stringr::str_detect("AB", "A")
  })

  expect_true(length(r$output$arguments) != 0)
})
