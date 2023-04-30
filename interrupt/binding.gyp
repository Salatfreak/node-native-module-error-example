{
  "targets": [
    {
      "target_name": "interrupt",
      "sources": [ "interrupt.cc" ],
      "include_dirs": ["<!(node -p \"require('node-addon-api').include_dir\")"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
    }
  ]
}
