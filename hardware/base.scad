include <chamfer.scad>;
include <threads.scad>;

inner_square=152.4;
border=6.35;
w=165.1;
h=193.0; //177.8;
thick=18.0;
square=50.8;
corner_hole_r=1.5;
floor_thick=1.5;

led_separation=16.9;
led_square=5.15;

button_r=5.75;
button_neck_thick=3.0;
button_neck_r=2.0;
button_fn=32;

battery_h=33.0;
battery_w=59.0;

strip_h=16.0;
strip_thick=2.0;
strip_notch_h=9.0;
strip_notch_w=5.0;

module resistor_pocket(height) {
    resistor_square=3.5;
    resistor_thick=1.0;

    translate([
        led_square/2+resistor_square/2-0.01,0,
        height/2+floor_thick-resistor_thick
    ])
        cube([
            resistor_square,
            resistor_square,
            height
        ], center=true);
}

module led_pair() {
    sufficient_height=10;
    
    translate([-led_separation/2,0]) {
        translate([0,0]) {
            cube([led_square,led_square,sufficient_height], center=true);
            resistor_pocket(sufficient_height);
        }
        translate([led_separation,0]) {
            cube([led_square,led_square,sufficient_height], center=true);
            resistor_pocket(sufficient_height);
        }

    }
}

module button() {
    inner_thick = floor_thick + button_neck_thick;
    //cylinder(r=button_r, h=inner_thick, $fn=button_fn);
    metric_thread (12, 1.0, inner_thick, internal=true);
}

module button_neck() {
    cylinder(r=button_r+button_neck_r, h=button_neck_thick, $fn=button_fn);
}

module battery_holder() {
    difference() {
        cube([battery_w+3, battery_h+3, thick]);
        translate([1.5, 1.5, -0.01])
            cube([battery_w, battery_h, thick+1]);
        
        for (y=[0:2]) {
            for (x=[0:2]) {
                translate([x*battery_w/2,y*battery_h/2])
                    translate([-battery_w/3/2,-battery_h/3/2])
                        cube([battery_w/3, battery_h/3,thick+1]);
            }
        }
    }
}


module add1(thickness) {
    //cube([w,h,2]);
    chamferCube([w,h,thickness], [[0,0,0,0],[0,0,0,0],[1,1,1,1]], ch=2);
}

module sub1() {
    translate([border, border, floor_thick])
        cube([inner_square, h-border*2, thick*2]);
}

module add2() {
    cyl_len = led_separation+led_square;
    cyl_r = 7;
    cyl_inner_r = 6;
    
    translate([border,border]) {
        for (y=[0:2]) {
            for (x=[0:2]) {
                translate([x*square+square/2,y*square+square/2,0]) {
                    translate([0,square/7])
                        button_neck();
                    // LED strip channel
                    translate([-(cyl_len)/2,-square/3])
                    rotate([0,90])
                    difference() {
                        cylinder(r=cyl_r,
                            h=cyl_len,
                            $fn=16
                        );
                        translate([0,0,-0.01])
                            cylinder(r=cyl_inner_r,
                                h=cyl_len+0.02,
                                $fn=16
                            );
                        // Cut bottom half of cylinder off
                        translate([-0.01,-cyl_r,-0.01])
                            cube([cyl_r*2,cyl_r*2,cyl_len+0.02]);

                        // Cut top part of cylinder off
                        translate([-cyl_r*2-floor_thick-cyl_r/3,-cyl_r,-0.01])
                            cube([cyl_r*2,cyl_r*2,cyl_len+0.02]);
                    }
                }
            }
        }
    }
}

module screw_holes() {
    // Screw holes
    for (y=[0:2]) {
        for (x=[0:2]) {
            translate([x*((w-border)/2)+border/2, y*((h-border)/2)+border/2,thick/2])
                if (x != 1 || y != 1)
                    cylinder(r=1.5,h=100,$fn=24);
        }
    }
}

module sub2() {
    // Carve squares and holes for LEDs and buttons
    translate([border, border]) {
        for (y=[0:2]) {
            for (x=[0:2]) {
                translate([x*square+square/2,y*square+square/2,-0.1]) {
                    translate([0,-square/3])
                        led_pair();
                    translate([0,square/7])
                        button();
                }
            }
        }
    }

    screw_holes();

    // Slot for micro-USB
    //26mm x 6mm, 4.65mm from edge
    translate([w-border-30-36.5,h-26-border-4.65,floor_thick+8.5])
        cube([30,80,60]);
}

module board_top() {
    difference() {
        union() {
            difference() {
                add1(thick);
                sub1();
            }
            add2();
        }
        sub2();
    }
}

module board_bottom() {
    difference() {
        add1(floor_thick);
        translate([0,0,-20])
            screw_holes();
    }
}

module led_strip_backing() {
    difference() {
        cube([inner_square, strip_h, strip_thick]);
        // Shaved Ends
        translate([-0.001,0,-0.01]) cube([0.05,strip_h,strip_thick*2]);
        translate([inner_square-0.05,0,-0.01]) cube([0.055,strip_h,strip_thick*2]);
        // Notches
        translate([0,strip_h/2-strip_notch_h/2,-0.01])
            cube([strip_notch_w, strip_notch_h, strip_thick*2]);
        translate([inner_square-strip_notch_w+0.01,strip_h/2-strip_notch_h/2,-0.01])
            cube([strip_notch_w, strip_notch_h, strip_thick*2]);
    }
}


//board_top();

board_bottom();

//led_strip_backing();
