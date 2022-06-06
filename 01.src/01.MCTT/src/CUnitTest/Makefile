#####################################################
# Target build configuration file
# (C)Copylight Resaved 2015 FUJIFILM Sofyware Inc.
#####################################################
#####################################################
# ビルドルールへのパス
#    RULE_PATHはmakeruleがある場所
#####################################################
RULE_PATH=..

#####################################################
# ディレクトリ設定(自動検索)
#   MY_DIRは自Makefileへの絶対パス
#   MY_DIR_NAMEは自Makefileのディレクトリ名(eg. aa/bb/cc-> cc)
#   MY_DIR_SUBは下位ディレクトリ一覧
# 自動検索したくない場合は手書きで記述する
#####################################################
MY_DIR=$(shell pwd)
MY_DIR_NAME=$(shell pwd | sed 's,^\(.*/\)\?\([^/]*\),\2,')
MY_DIR_SUB=$(shell find -type d | sed ':loop; N; $!b loop; s/\n/ /g')

#####################################################
# コンパイル対象のソースファイル（自動検索）
# 自動検索したくない場合は手書きで記述する
#####################################################
MY_VPATH=$(MY_DIR_SUB)

MY_TARGET_SRC_CMD=$(patsubst %,%/*.c,$(MY_DIR_SUB))
MY_TARGET_SRC_CMD+=$(patsubst %,%/*.cpp,$(MY_DIR_SUB))
MY_TARGET_SRC_CMD+=$(patsubst %,%/*.cxx,$(MY_DIR_SUB))
MY_TARGET_SRC=$(wildcard $(MY_TARGET_SRC_CMD))

#####################################################
# 出力ファイルの格納先
#    MY_TARGET_PATHは当該makefileで作成したファイルの格納場所
#    MY_OBJECT_PATHは当該makefileで作成したオブジェクトの格納場所
#    MY_DEPEND_PATHは当該makefileで作成したヘッダ依存ファイルの格納場所
#####################################################
MY_TARGET_PATH=$(RULE_PATH)/_release/lib
MY_OBJECT_PATH=$(RULE_PATH)/_release/obj/$(MY_DIR_NAME)
MY_DEPEND_PATH=$(RULE_PATH)/_release/dep/$(MY_DIR_NAME)

#####################################################
# 作成するファイル名および種類
#  TARGETは作成するファイル名
#  TARGET_TYPEは作成ファイルタイプ（ライブラリ(LIB)or実行ファイル(EXE)）
#####################################################
MY_TARGET= $(MY_TARGET_PATH)/libFF_$(MY_DIR_NAME).a
MY_TARGET_TYPE=LIB

#####################################################
#  当該makefileでのみリンクするライブラリの場所とライブラリ名
#  Target_TYPE=LIBの場合は無効
#  "-L"、"-l"は付けないで記述
#####################################################
MY_TARGET_LIBPATH=
MY_TARGET_LIB += FF_FFW

#####################################################
#  当該makefileでのみ付加するコンパイルオプション
#####################################################
MY_TARGET_FLAGS=

#####################################################
#  当該makefileでのみ付加するコンパイルスイッチ用デファイン
#  "-D"は付けないで記述
#####################################################
MY_TARGET_DEFINES=

#####################################################
#  当該makefileでのみ検索するインクルードパス
#  "-I"は付けないで記述
#####################################################
INCLUDE_PATH=.
MY_TARGET_INCLUDES+=$(INCLUDE_PATH)                       \
                    $(RULE_PATH)/include                  \
                    $(RULE_PATH)/SwLib                    \
                    $(RULE_PATH)/CUnitTest                \
                    $(RULE_PATH)/FFW/API                  \
                    $(RULE_PATH)/FFW/include

#####################################################
# ビルド共通ルール（変更禁止）
#   RULE_PATHが間違っている場合はエラーになる
#####################################################
include $(RULE_PATH)/makerule.common

#####################################################
# End of File
#####################################################

