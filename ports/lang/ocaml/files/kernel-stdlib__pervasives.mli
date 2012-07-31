--- ./stdlib/pervasives.mli.orig	2011-05-17 15:31:32.000000000 +0200
+++ ./stdlib/pervasives.mli	2012-07-27 18:05:28.081958765 +0200
@@ -261,74 +261,74 @@
 external ( /. ) : float -> float -> float = "%divfloat"
 (** Floating-point division. *)
 
-external ( ** ) : float -> float -> float = "caml_power_float" "pow" "float"
+external ( ** ) : float -> float -> float = "caml_power_float"
 (** Exponentiation. *)
 
-external sqrt : float -> float = "caml_sqrt_float" "sqrt" "float"
+external sqrt : float -> float = "caml_sqrt_float"
 (** Square root. *)
 
-external exp : float -> float = "caml_exp_float" "exp" "float"
+external exp : float -> float = "caml_exp_float"
 (** Exponential. *)
 
-external log : float -> float = "caml_log_float" "log" "float"
+external log : float -> float = "caml_log_float"
 (** Natural logarithm. *)
 
-external log10 : float -> float = "caml_log10_float" "log10" "float"
+external log10 : float -> float = "caml_log10_float"
 (** Base 10 logarithm. *)
 
-external expm1 : float -> float = "caml_expm1_float" "caml_expm1" "float"
+external expm1 : float -> float = "caml_expm1_float"
 (** [expm1 x] computes [exp x -. 1.0], giving numerically-accurate results
     even if [x] is close to [0.0].
     @since 3.12.0
 *)
 
-external log1p : float -> float = "caml_log1p_float" "caml_log1p" "float"
+external log1p : float -> float = "caml_log1p_float"
 (** [log1p x] computes [log(1.0 +. x)] (natural logarithm),
     giving numerically-accurate results even if [x] is close to [0.0].
     @since 3.12.0
 *)
 
-external cos : float -> float = "caml_cos_float" "cos" "float"
+external cos : float -> float = "caml_cos_float"
 (** Cosine.  Argument is in radians. *)
 
-external sin : float -> float = "caml_sin_float" "sin" "float"
+external sin : float -> float = "caml_sin_float"
 (** Sine.  Argument is in radians. *)
 
-external tan : float -> float = "caml_tan_float" "tan" "float"
+external tan : float -> float = "caml_tan_float"
 (** Tangent.  Argument is in radians. *)
 
-external acos : float -> float = "caml_acos_float" "acos" "float"
+external acos : float -> float = "caml_acos_float"
 (** Arc cosine.  The argument must fall within the range [[-1.0, 1.0]].
     Result is in radians and is between [0.0] and [pi]. *)
 
-external asin : float -> float = "caml_asin_float" "asin" "float"
+external asin : float -> float = "caml_asin_float"
 (** Arc sine.  The argument must fall within the range [[-1.0, 1.0]].
     Result is in radians and is between [-pi/2] and [pi/2]. *)
 
-external atan : float -> float = "caml_atan_float" "atan" "float"
+external atan : float -> float = "caml_atan_float"
 (** Arc tangent.
     Result is in radians and is between [-pi/2] and [pi/2]. *)
 
-external atan2 : float -> float -> float = "caml_atan2_float" "atan2" "float"
+external atan2 : float -> float -> float = "caml_atan2_float"
 (** [atan2 y x] returns the arc tangent of [y /. x].  The signs of [x]
     and [y] are used to determine the quadrant of the result.
     Result is in radians and is between [-pi] and [pi]. *)
 
-external cosh : float -> float = "caml_cosh_float" "cosh" "float"
+external cosh : float -> float = "caml_cosh_float"
 (** Hyperbolic cosine.  Argument is in radians. *)
 
-external sinh : float -> float = "caml_sinh_float" "sinh" "float"
+external sinh : float -> float = "caml_sinh_float"
 (** Hyperbolic sine.  Argument is in radians. *)
 
-external tanh : float -> float = "caml_tanh_float" "tanh" "float"
+external tanh : float -> float = "caml_tanh_float"
 (** Hyperbolic tangent.  Argument is in radians. *)
 
-external ceil : float -> float = "caml_ceil_float" "ceil" "float"
+external ceil : float -> float = "caml_ceil_float"
 (** Round above to an integer value.
     [ceil f] returns the least integer value greater than or equal to [f].
     The result is returned as a float. *)
 
-external floor : float -> float = "caml_floor_float" "floor" "float"
+external floor : float -> float = "caml_floor_float"
 (** Round below to an integer value.
     [floor f] returns the greatest integer value less than or
     equal to [f].
@@ -337,7 +337,7 @@
 external abs_float : float -> float = "%absfloat"
 (** [abs_float f] returns the absolute value of [f]. *)
 
-external mod_float : float -> float -> float = "caml_fmod_float" "fmod" "float"
+external mod_float : float -> float -> float = "caml_fmod_float"
 (** [mod_float a b] returns the remainder of [a] with respect to
    [b].  The returned value is [a -. n *. b], where [n]
    is the quotient [a /. b] rounded towards zero to an integer. *)
