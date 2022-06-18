package com.xzc.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@SuppressWarnings("all")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Course {
    /**
     * 课程代号
     */
    private String Cno;
    /**
     * 课程名字
     */
    private String Cname;
    /**
     * 先修课程
     */
    private String Cpno;
    /**
     * 学分
     */
    private Integer Ccredit;

}
