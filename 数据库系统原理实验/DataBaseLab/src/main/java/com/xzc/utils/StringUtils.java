package com.xzc.utils;


import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.IntStream;

@SuppressWarnings("all")
public class StringUtils {

    /**
     * 判断字符串是否为空
     *
     * @param str
     * @return
     */
    public static boolean isEmpty(String str) {
        return str == null || "".equals(str);
    }

    /**
     * 将content按照正则匹配，返回可以匹配的字符串列表
     *
     * @param reg
     * @param content
     * @return
     */
    public static List<String> extractMessage(String reg, String content) {
        Pattern compile = Pattern.compile(reg, Pattern.CASE_INSENSITIVE);
        Matcher matcher = compile.matcher(content);
        List<String> list = new ArrayList<>();
        while (matcher.find()) {
            list.add(matcher.group());
        }
        return list;
    }


    /**
     * 将str重复count次，返回结果
     *
     * @param str
     * @param count
     * @return
     */
    public static String getRepeatChar(String str, int count) {
        StringBuilder res = new StringBuilder();
        IntStream.range(0, count).forEach(i -> res.append(str));
        return res.toString();
    }

    /**
     * 将字符串填充到指定长度并居中对齐
     *
     * @param str
     * @param len
     * @return
     */
    public static String getPadString(String str, Integer len) {
        StringBuilder res = new StringBuilder();
        str = str.trim();
        if (str.length() < len) {
            int diff = len - str.length();
            int fixLen = diff / 2;
            String fix = getRepeatChar(" ", fixLen);
            res.append(fix).append(str).append(fix);
            if (res.length() > len) {
                return res.substring(0, len);
            } else {
                res.append(getRepeatChar(" ", len - res.length()));
                return res.toString();
            }
        }
        return str.substring(0, len);
    }

    /**
     * 此方法主要为表格的单元格数据按照指定长度填充并居中对齐并带上分割符号
     *
     * @param str    原始字符串
     * @param len    输出字符串的总长度
     * @param symbol 分割符号
     * @param index  传入的cell在list的索引，如果为第一个则需要在前面增加分割符号
     * @return
     */
    public static String getPadString(String str, Integer len, String symbol, int index) {
        String origin = str + "  ";
        if (index == 0) {
            String tmp = getPadString(origin, len - 2);
            return symbol + tmp + symbol;
        } else {

            String tmp = getPadString(origin, len - 1);
            return tmp + symbol;
        }
    }

    /**
     * 得到一个字符串中单字节出现的次数
     *
     * @param cell
     * @return
     */
    public static Integer getENCharCount(String cell) {
        if (cell == null) {
            return 0;
        }
        String reg = "[^\t\\x00-\\xff]";
        return cell.replaceAll(reg, "").length();
    }

    /**
     * 得到制表符长度，每个\t显示四个长度
     *
     * @param cell
     * @return
     */
    public static Integer getTableCount(String cell) {
        if (cell == null) {
            return 0;
        }
        String reg = "\t";
        return cell.length() - cell.replaceAll(reg, "").length();
    }

    /**
     * 得到一个字符串中双字节出现的次数
     *
     * @param cell
     * @return
     */
    public static Integer getZHCharCount(String cell) {
        if (cell == null) {
            return 0;
        }
        return cell.length() - getENCharCount(cell);
    }


}

