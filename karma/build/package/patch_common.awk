/\$\(addprefix \$\(T_SRC_DIR\)/   { next }
/\$\(addprefix -I\$\(T_SRC_DIR\)/ { sub("\\$\\(addprefix -I\\$\\(T_SRC_DIR\\)", "$(addprefix -I") }
/@ME_INC_PATH@/                   { if (FORCED_INC != "") gsub("@ME_INC_PATH@", FORCED_INC) }
/@ME_LIB_PATH@/                   { if (FORCED_LIB != "") gsub("@ME_LIB_PATH@", FORCED_LIB) }

{print}