
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; MODULE      : drd-test.scm
;; DESCRIPTION : examples of logical programs
;; COPYRIGHT   : (C) 2003  Joris van der Hoeven
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; This software falls under the GNU general public license and comes WITHOUT
;; ANY WARRANTY WHATSOEVER. See the file $TEXMACS_PATH/LICENSE for details.
;; If you don't have this file, write to the Free Software Foundation, Inc.,
;; 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(texmacs-module (kernel drd drd-test)
  (:use (kernel drd drd-rules) (kernel drd drd-query)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Examples of rules
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(drd-rules
  ((sun% Joris Piet))
  ((sun% Piet Opa))
  ((daughter% Geeske Opa))
  ((daughter% Jekke Opa))
  ((child% 'x 'y) (sun% 'x 'y))
  ((child% 'x 'y) (daughter% 'x 'y))
  ((descends% 'x 'y) (child% 'x 'y))
  ((descends% 'x 'z) (child% 'x 'y) (descends% 'y 'z)))

;; (??? (child% 'x Opa))
;; (??? (descends% 'x 'y))
;; (??? (descends% 'x 'y) (daughter% Joleen Opa))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Addional assumptions <-> creating modules
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(drd-rules
  (assume family%) ; added to constraints for all rules below
  ((sun% Joris Piet))
  ((sun% Piet Opa))
  ((daughter% Geeske Opa))
  ((daughter% Jekke Opa))
  ((child% 'x 'y) (sun% 'x 'y))
  ((child% 'x 'y) (daughter% 'x 'y))
  ((descends% 'x 'y) (child% 'x 'y))
  ((descends% 'x 'z) (child% 'x 'y) (descends% 'y 'z)))

;; (??? (child% 'x Opa))
;; (??? (child% 'x Opa) family%)
