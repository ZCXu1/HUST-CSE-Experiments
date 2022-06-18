package com.xzc.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@SuppressWarnings("all")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Student {
    /**
     * 学号
     */
    private String Sno;
    /**
     * 姓名
     */
    private String Sname;
    /**
     * 性别
     */
    private String Ssex;
    /**
     * 年龄
     */
    private Integer Sage;
    /**
     * 专业
     */
    private String Sdept;
    /**
     * 奖学金
     */
    private String Scholarship;

}
