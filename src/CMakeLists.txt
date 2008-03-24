SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)
include(${QT_USE_FILE})

set(yaqftp_SRCS
  main.cpp 
  mainwindow.cpp 
  bookmarksdialog.cpp 
  ftpsessiondialog.cpp 
  filemodel.cpp 
  ftpmodel.cpp 
  quickconnectiondialog.cpp 
)
SET(yaqftp_MOCS
  bookmark.h
  bookmarksdialog.h
  filemodel.h
  ftpmodel.h
  ftpsessiondialog.h
  mainwindow.h
  quickconnectiondialog.h
)
set(yaqftp_RCCS resources.qrc)

qt4_wrap_cpp(yaqftp_MOC_SRCS ${yaqftp_MOCS})
qt4_add_resources(yaqftp_RCC_SRCS ${yaqftp_RCCS})

# http://www.cmake.org/Wiki/KWWidgets/i18n#Qt
add_executable(yaqftp ${yaqftp_SRCS} ${yaqftp_MOC_SRCS} ${yaqftp_RCC_SRCS})
target_link_libraries(yaqftp ${QT_LIBRARIES})
