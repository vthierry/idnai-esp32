
define package
{
  login: vthierry
  logo: "docs/idnai-logo-purple.png"
  keywords: [ esp32, web-service, weak-json ]
  dependencies: [ idnai-make ]
  os: [ esp32 ]
}
endef

include node_modules/idnai-make/src/makefile-rules.mk
